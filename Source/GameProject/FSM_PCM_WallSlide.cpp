// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM_PCM_WallSlide.h"
#include "PlayerCharacterMovementComponent.h"
#include "Engine/World.h"
#include "HelperFunctions.h"
#include "FSM_PCM_Run.h"
#include "FSM_PCM_Slide.h"
#include "FSM_PCM_Air.h"
#include "Components/BoxComponent.h"


void UFSM_PCM_WallSlide::Initialize(UPlayerCharacterMovementComponent* comp)
{
	UE_LOG(LogTemp, Warning, TEXT("[%f]UFSM_PCM_WallSlide::Initialize"), comp->GetWorld()->GetTimeSeconds());

	comp->StopMovement();
}

UObject* UFSM_PCM_WallSlide::HandleInput(UPlayerCharacterMovementComponent* comp, APlayerCharacterController::CharacterInput input, float axis)
{
	switch (input)
	{
	case APlayerCharacterController::CharacterInput::moveLeftRight:
		break;
	case APlayerCharacterController::CharacterInput::moveDownUp:
		break;
	case APlayerCharacterController::CharacterInput::jump:
		return HandleJump(comp);
		break;
	case APlayerCharacterController::CharacterInput::fire:
		break;
	case APlayerCharacterController::CharacterInput::possess:
		break;
	default:
		break;
	}

	return nullptr;
}

UObject* UFSM_PCM_WallSlide::Tick(UPlayerCharacterMovementComponent* comp, float dt)
{
	UE_LOG(LogTemp, Warning, TEXT("[%f]Curr wall velocity: %f"), comp->GetWorld()->GetTimeSeconds(), *comp->GetVelocity().ToString());


	UObject* pObject = HandleSlideDown(comp, dt);
	if (pObject) return pObject;

	if (!IsWallDetected(comp, dt))
	{
		return TransitionToFall(comp);
	}

	return nullptr;
}

void UFSM_PCM_WallSlide::SetWallDirection(const FVector& dir)
{
	m_WallDir = dir.GetSafeNormal();
	FVector wallNormal = m_WallDir * (-1.0f);
	float slopeAngle = HelpFunc::CalculateSlopeAngleDegrees(wallNormal);
	FVector slideDir = FVector::CrossProduct(FVector::RightVector, wallNormal);

	if (slideDir.Z > 0.0f) m_SlideDirection = slideDir * (-1.0f);
	else m_SlideDirection = slideDir;

	UE_LOG(LogTemp, Warning, TEXT("Wall Slide Direction: %s"), *slideDir.ToString());

}

void UFSM_PCM_WallSlide::Exit(UPlayerCharacterMovementComponent* comp)
{

}

UObject* UFSM_PCM_WallSlide::HandleSlideDown(UPlayerCharacterMovementComponent* comp, float dt)
{
	AActor* pOwner = comp->GetOwner();
	if (!pOwner) return nullptr;

	//Handle downward Acceleration
	float fallSpeed = comp->GetWallSlideMaxSpeed();
	//float currSpeed = comp->GetVelocity().Z;
	//currSpeed -= (comp->GetWallSlideAcceleration() * dt);
	//currSpeed = FMath::Clamp(currSpeed, -fallSpeed, fallSpeed);

	float currSpeed = comp->GetVelocity().Size();
	currSpeed -= (comp->GetWallSlideAcceleration() * dt);
	//currSpeed = FMath::Clamp(currSpeed, -fallSpeed, fallSpeed);
	FVector newVel = comp->GetVelocity() +  m_SlideDirection * comp->GetWallSlideAcceleration() * dt;
	newVel = newVel.GetClampedToMaxSize(fallSpeed);

	//If ground is hit, change state
	FHitResult hitResult{};
	//if (!comp->AddWorldOffset(FVector(0.0f, 0.0f, currSpeed * dt), hitResult) && (currSpeed < 0.0f))
	if (!comp->AddWorldOffset(newVel * dt, hitResult) && (newVel.Z < 0.0f))
	{
		float slopeAngle = HelpFunc::CalculateSlopeAngleDegrees(hitResult.ImpactNormal);
		FVector dir = FVector::CrossProduct(comp->GetPositiveYAxis(), hitResult.ImpactNormal);

		//If to steep, transition to Slide State
		if (slopeAngle > comp->GetMaxSlopeAngle())
		{
			return nullptr;
			//return TransitionToSlide(comp, dir);
		}

		//If not to steep, transition to Run	
		return TransitionToRun(comp, dir);
	}

	//Set new velocity Z if no ground is hit
	//comp->SetVelocityZ(currSpeed);
	comp->SetVelocity(newVel);
	return nullptr;
}

UObject* UFSM_PCM_WallSlide::HandleJump(UPlayerCharacterMovementComponent* comp)
{
	UWorld* pWorld = comp->GetWorld();
	if (!pWorld) return nullptr;

	float horSpeed = comp->GetWallHorJumpSpeed();
	float vertSpeed = comp->GetWallVertJumpSpeed();

	FVector jumpVel{ m_WallDir.X * horSpeed * (-1.0f), m_WallDir.Y * horSpeed * (-1.0f), vertSpeed }; // *-1.0f to jump away from wall

	comp->SetVelocity(jumpVel);

	float dt = pWorld->GetDeltaSeconds();
	comp->AddWorldOffset(jumpVel * dt);

	UFSM_PCM_Air* pAir = NewObject<UFSM_PCM_Air>();
	if (!pAir) return nullptr;
	pAir->SetNrJumpLeft(comp->GetMaxNrOfJumps() - 1);

	comp->OnJump.Broadcast();

	return pAir;
}

bool UFSM_PCM_WallSlide::IsWallDetected(UPlayerCharacterMovementComponent* comp, float dt)
{
	float rayLength{ 2.0f };
	return IsWallHitByRayTraceBox(comp, dt, rayLength);
}

UObject* UFSM_PCM_WallSlide::TransitionToRun(UPlayerCharacterMovementComponent* comp, FVector newDir)
{
	comp->StopMovementZ();
	UFSM_PCM_Run* pRun = NewObject<UFSM_PCM_Run>();
	if (pRun) pRun->SetPosDirection(newDir);
	Exit(comp);

	comp->OnRun.Broadcast();

	return pRun;
}

UObject* UFSM_PCM_WallSlide::TransitionToFall(UPlayerCharacterMovementComponent* comp)
{
	UFSM_PCM_Air* pAir = NewObject<UFSM_PCM_Air>();
	if (!pAir) return nullptr;
	pAir->SetNrJumpLeft(comp->GetMaxNrOfJumps());
	Exit(comp);

	comp->OnFall.Broadcast();

	return pAir;
}

UObject* UFSM_PCM_WallSlide::TransitionToJump(UPlayerCharacterMovementComponent* comp)
{
	UWorld* pWorld = comp->GetWorld();
	if (!pWorld) return nullptr;

	float horSpeed = comp->GetWallHorJumpSpeed();
	float vertSpeed = comp->GetWallVertJumpSpeed();

	FVector jumpVel{ m_WallDir.X * horSpeed, m_WallDir.Y * horSpeed, vertSpeed };
	FVector vel = comp->GetVelocity();

	comp->SetVelocity(jumpVel);

	UFSM_PCM_Air* pAir = NewObject<UFSM_PCM_Air>();
	if (!pAir) return nullptr;
	pAir->SetNrJumpLeft(comp->GetMaxNrOfJumps() - 1);

	comp->OnJump.Broadcast();

	return pAir;
}

bool UFSM_PCM_WallSlide::IsWallHitByRayTraceBox(UPlayerCharacterMovementComponent* comp, float dt, float rayLength)
{
	//Check Actor
	AActor* pOwner = comp->GetOwner();
	if (!pOwner) return false;

	//CheckCapsule
	UBoxComponent* pBox = pOwner->FindComponentByClass<UBoxComponent>();
	if (!pBox) return false;

	//Check World
	UWorld* pWorld = comp->GetWorld();
	if (!pWorld) return false;

	//Start Ray Trace with capsule shape
	FHitResult hitResult{};
	FCollisionQueryParams params{};
	params.bTraceComplex = false;
	params.AddIgnoredActor(pOwner);

	bool isHit{};
	isHit = pWorld->SweepSingleByChannel
	(
		hitResult,
		pOwner->GetActorLocation(),
		pOwner->GetActorLocation() + m_WallDir * rayLength,
		pOwner->GetActorRotation().Quaternion(),
		ECollisionChannel::ECC_Visibility,
		FCollisionShape::MakeBox(pBox->GetScaledBoxExtent()),
		params
	);

	if (isHit)
	{
		return true;
	}

	return false;
}