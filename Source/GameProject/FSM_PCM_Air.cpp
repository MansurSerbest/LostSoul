// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM_PCM_Air.h"
#include "FSM_PCM_Run.h"
#include "FSM_PCM_Idle.h"
#include "PlayerCharacterMovementComponent.h"
#include "GameFramework/Actor.h"
#include "PlayerCharacterController.h"
#include "Engine/World.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "HelperFunctions.h"
#include "FSM_PCM_Slide.h"
#include "FSM_PCM_WallSlide.h"
#include "DimensionMesh.h"
#include "PlayerCharacter.h"

void UFSM_PCM_Air::Initialize(UPlayerCharacterMovementComponent* comp)
{
	UE_LOG(LogTemp, Warning, TEXT("[%f]UFSM_PCM_Air::Initialize"), comp->GetWorld()->GetTimeSeconds());

	//Check Actor
	AActor* pOwner = comp->GetOwner();
	if (!pOwner) return;

	//Register Function to Event
	UBoxComponent* pBox = pOwner->FindComponentByClass<UBoxComponent>();
	if (pBox)
		pBox->OnComponentHit.AddDynamic(this, &UFSM_PCM_Air::HitGround);


}

UObject* UFSM_PCM_Air::HandleInput(UPlayerCharacterMovementComponent* comp, APlayerCharacterController::CharacterInput input, float axis)
{
	switch (input)
	{
	case APlayerCharacterController::CharacterInput::moveLeftRight:
	{
		//Set flag if axis value not bigger than deadzone
		float axisDeadZone{ 0.01f };
		if (FMath::Abs(axis) < axisDeadZone)
			m_IsAxisInput = false;
		else
			m_IsAxisInput = true;

		return HandleLeftRight(comp, axis);
		break;
	}
	case APlayerCharacterController::CharacterInput::jump:
		return HandleJump(comp);
		break;
	default:
		return nullptr;
	}

	return nullptr;
}

UObject* UFSM_PCM_Air::Tick(UPlayerCharacterMovementComponent* comp, float dt)
{
	UObject* pObject = HandleUpDown(comp, dt);
	if (pObject)
	{
		return pObject; //If ground hit, change state to groundState: Idle or Running
	}

	DampActor(comp, dt);
	m_IsAxisInput = false;

	if (comp->GetVelocityXY() != FVector::ZeroVector)
		comp->SetFacingDirection(comp->GetVelocityXY().GetSafeNormal());//the facing direction is horizontal, so no need for the Z-value

	return nullptr;
}

void UFSM_PCM_Air::SetNrJumpLeft(int32 nr)
{
	m_NrJumpLeft = nr;
}

void UFSM_PCM_Air::Exit(UPlayerCharacterMovementComponent* comp)
{
	//Check Actor
	AActor* pOwner = comp->GetOwner();
	if (!pOwner) return;

	//Register Function to Event
	UBoxComponent* pBox = pOwner->FindComponentByClass<UBoxComponent>();
	if (pBox)
		pBox->OnComponentHit.RemoveDynamic(this, &UFSM_PCM_Air::HitGround);
}

UObject* UFSM_PCM_Air::HandleLeftRight(UPlayerCharacterMovementComponent* comp, float axis)
{
	//Get data
	FVector vel = comp->GetVelocityXY();
	float maxAcc = comp->GetMaxAccelerationXY();
	float maxSpeed = comp->GetMaxSpeedXY();

	//Get delta time
	UWorld* pWorld = comp->GetWorld();
	if (!pWorld) return nullptr;
	float dt = pWorld->GetDeltaSeconds();

	//Get standard forward direction and multiply it with axis and acc..
	vel += comp->GetPosMoveDirection() * maxAcc * axis * dt;
	vel = vel.GetClampedToMaxSize(maxSpeed);

	FHitResult hitResult{};
	if (!comp->AddWorldOffset(comp->GetPosMoveDirection() * vel * dt, hitResult))
	{
		if (hitResult.Component->ComponentHasTag("Wall"))
		{
			//If wall detected, transition to wall slide
			float angle{ HelpFunc::CalcAngleDegrees(hitResult.ImpactNormal, comp->GetPosMoveDirection()) };
			float wallAngle = HelpFunc::CalculateSlopeAngleDegrees(hitResult.ImpactNormal);

			if (wallAngle > comp->GetMaxSlopeAngle())
			{
				return TransitionToWallSlide(comp, hitResult.ImpactNormal * (-1.0f));
			}
		}

		if (hitResult.Component->ComponentHasTag("Slope"))
		{
			//If it is not wall, check if it is a steep slope
			AActor* pOwner = comp->GetOwner();
			if (!pOwner)
			{
				comp->SetVelocityXY(vel);
				return nullptr;
			}

			float slopeAngle = HelpFunc::CalculateSlopeAngleDegrees(hitResult.ImpactNormal);
			FVector dir = FVector::CrossProduct(comp->GetPositiveYAxis(), hitResult.ImpactNormal);

			if (slopeAngle > comp->GetMaxSlopeAngle())
				return TransitionToSlide(comp, dir);
		}
	}

	comp->SetVelocityXY(vel);
	return nullptr;
}

void UFSM_PCM_Air::DampActor(UPlayerCharacterMovementComponent* comp, float dt)
{
	if (m_IsAxisInput)
	{
		return;
	}

	//If speed is under a certain treshold, stop the movement completely
	float minSpeed{ 0.1 };
	if (comp->GetSpeedXY() < minSpeed)
	{
		comp->StopMovementXY();
		return;
	}

	//Get data
	FVector vel = comp->GetVelocityXY();
	float damp = comp->GetAirDampingXY();
	float maxSpeed = comp->GetMaxSpeedXY();

	//Get velocity and take the opposite direction. Add the world offset
	FVector offset = vel.GetSafeNormal();
	offset *= (-1.0f) * damp * dt;
	offset = offset.GetClampedToSize(0.0f, vel.Size());

	//If velocity is smaller than 100 m/s, stop movement completely
	float stopTreshold{ 10.0f };
	FVector diff = vel + offset;

	if (comp->AddWorldOffset(offset * dt)) //EXECUTE MOVEMENT
		comp->SetVelocityXY(diff);
}

UObject* UFSM_PCM_Air::HandleUpDown(UPlayerCharacterMovementComponent* comp, float dt)
{
	float vel = comp->GetVelocity().Z;

	//Apply Gravity
	float grav = comp->GetGravity();
	vel += (-1.0f) * grav * dt;

	AActor* pOwner = comp->GetOwner();
	if (!pOwner) return nullptr;

	bool hitCeiling{ false };

	//If ground is hit, change state
	FHitResult hitResult{};
	if (!comp->AddWorldOffset(FVector(0.0f, 0.0f, vel * dt), hitResult))
	{
		float slopeAngle = HelpFunc::CalculateSlopeAngleDegrees(hitResult.ImpactNormal);
		FVector dir = FVector::CrossProduct(comp->GetPositiveYAxis(), hitResult.ImpactNormal);
		UE_LOG(LogTemp, Warning, TEXT("Slopeangle: %f"), slopeAngle);
		//If to steep, transition to Slide State
		if (hitResult.Component->ComponentHasTag("Wall") && (slopeAngle > comp->GetMaxSlopeAngle()))
		{
			return TransitionToWallSlide(comp, hitResult.ImpactNormal * (-1.0f));
			//return pSlide;
		}

		//If not to steep, transition to Run	
		if (comp->GetVelocity().Z <= 0.0f)
		{
			APlayerCharacter* pPlayerChar = Cast<APlayerCharacter>(comp->GetOwner());
			UE_LOG(LogTemp, Warning, TEXT("dimensionmesh collision"));
			if (hitResult.GetActor()->IsA<ADimensionMesh>())
			{
				comp->SetIsOnMovingPlatform(true);
				UE_LOG(LogTemp, Warning, TEXT("landed on dimension platform"));
				Cast<ADimensionMesh>(hitResult.GetActor())->SetCharacter(pPlayerChar);
			}
			if (comp->GetVelocityXY() == FVector::ZeroVector) return TransitionToIdle(comp);
			return TransitionToRun(comp);
		}


		//If Z is not smaller than zero, it means it hit a ceiling, therefore set Z to zero to reset the Z value
		hitCeiling = true;
	}

	//Set new velocity Z
	if (!hitCeiling) comp->SetVelocityZ(vel);
	else comp->SetVelocityZ(0.0f);
	return nullptr;
}

UObject* UFSM_PCM_Air::HandleJump(UPlayerCharacterMovementComponent* comp, float axis)
{
	if (m_NrJumpLeft > 0)
	{
		comp->SetVelocityZ(comp->GetJumpSpeed());
		--m_NrJumpLeft;
		comp->OnJump.Broadcast();
	}

	return nullptr;
}

void UFSM_PCM_Air::HitGround(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AActor* pOwner = HitComponent->GetOwner();
	if (!pOwner) return;

	UPlayerCharacterMovementComponent* pMovComp = pOwner->FindComponentByClass<UPlayerCharacterMovementComponent>();
	if (!pMovComp) return;

	float x = (Hit.TraceEnd - Hit.TraceStart).X;
	float y = (Hit.TraceEnd - Hit.TraceStart).Y;


	if (pMovComp->GetVelocity().Z < 0.0f && x == 0.0f && y == 0.0f)
	{
		FVector cross = FVector::CrossProduct(pMovComp->GetPositiveYAxis(), Hit.ImpactNormal);
		m_GroundDirection = cross;
	}
}

UObject* UFSM_PCM_Air::TransitionToSlide(UPlayerCharacterMovementComponent* comp, FVector newDir)
{
	if (newDir.Z > 0.0f) newDir *= -1.0f; //Make sure that the slope direction is pointing downwards
	UFSM_PCM_Slide* pSlide = NewObject<UFSM_PCM_Slide>();
	pSlide->SetDirection(newDir);
	comp->SetFacingDirection(newDir);
	Exit(comp);

	comp->OnSlide.Broadcast();

	return pSlide;
}

UObject* UFSM_PCM_Air::TransitionToRun(UPlayerCharacterMovementComponent* comp)
{
	comp->StopMovementZ();
	UFSM_PCM_Run* pRun = NewObject<UFSM_PCM_Run>();
	if (pRun) pRun->SetPosDirection(m_GroundDirection);
	if (FVector::DotProduct(comp->GetVelocity(), m_GroundDirection) < 0.0f)
		comp->SetVelocity(comp->GetVelocity().Size() * (-1.0f) * comp->GetLandingVelocityRatio() * m_GroundDirection);
	else
		comp->SetVelocity(comp->GetVelocity().Size() * comp->GetLandingVelocityRatio() * m_GroundDirection);
	Exit(comp);

	comp->OnRun.Broadcast();

	return pRun;
}

UObject* UFSM_PCM_Air::TransitionToIdle(UPlayerCharacterMovementComponent* comp)
{
	comp->StopMovementZ();
	UFSM_PCM_Idle* pIdle = NewObject<UFSM_PCM_Idle>();
	if (!pIdle) return nullptr; 
	pIdle->SetPosDirection(m_GroundDirection);
	FVector newFaceDir{};
	FVector::DotProduct(m_GroundDirection, comp->GetFacingDirection()) < 0.0f ?
		newFaceDir = m_GroundDirection * (-1.0f) : newFaceDir = m_GroundDirection;
	//comp->SetFacingDirection(newFaceDir);
	Exit(comp);

	comp->OnIdle.Broadcast();

	return pIdle;
}

UObject* UFSM_PCM_Air::TransitionToWallSlide(UPlayerCharacterMovementComponent* comp, const FVector& wallDir)
{
	UFSM_PCM_WallSlide* pWallSlide = NewObject<UFSM_PCM_WallSlide>();
	if (!pWallSlide) return nullptr;
	pWallSlide->SetWallDirection(wallDir);
	comp->StopMovementXY();
	comp->SetVelocityZ(comp->GetVelocity().Z * 0.6f);
	Exit(comp);

	comp->OnWallSlide.Broadcast();

	return pWallSlide;
}