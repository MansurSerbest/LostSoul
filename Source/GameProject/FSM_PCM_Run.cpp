// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM_PCM_Run.h"
#include "PlayerCharacterMovementComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "FSM_PCM_Air.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "HelperFunctions.h"
#include "FSM_PCM_Slide.h"
#include "FSM_PCM_Idle.h"


void UFSM_PCM_Run::Initialize(UPlayerCharacterMovementComponent* comp)
{
	UE_LOG(LogTemp, Warning, TEXT("[%f]UFSM_PCM_Run::Initialize"), comp->GetWorld()->GetTimeSeconds());

	m_IsTouchingGround = true;
	m_IsAxisInput = false;
}

UObject* UFSM_PCM_Run::HandleInput(UPlayerCharacterMovementComponent* comp, APlayerCharacterController::CharacterInput input, float axis)
{
	switch (input)
	{
	case APlayerCharacterController::CharacterInput::moveLeftRight:
	{
		//Set flag if axis value not bigger than deadzone

		float newAxis{};
		float deadZone = comp->GetDeadZoneLeftX();
		if (FMath::Abs(axis) < deadZone)
		{
			m_IsAxisInput = false;
			newAxis = 0.0f;
		}
		else
		{
			m_IsAxisInput = true;
			//The axis should be a value between 0.0 and 1.0, but the deadzone is from 0.25. So whe need to convert the values between 
			//0.25 and 1.0f to a new value between 0.0 and 1.0
			newAxis = (FMath::Abs(axis) - deadZone) / (1.0f - deadZone) * FMath::Sign(axis);
		}

		m_CurrAxis = newAxis;
		return HandleLeftRight(comp, newAxis);
		break;
	}
	case APlayerCharacterController::CharacterInput::jump:
		return HandleJump(comp);
		break;
	default:
		return nullptr;
	}
}

UObject* UFSM_PCM_Run::Tick(UPlayerCharacterMovementComponent* comp, float dt)
{

	if (!m_IsTouchingGround)
	{
		comp->SetVelocityZ(0.0f);
		comp->OnFall.Broadcast();
		return NewObject<UFSM_PCM_Air>();
	}

	UObject* pObject = HandleUpDown(comp, dt);
	if (pObject)
	{
		return pObject;
	}

	pObject = DampActor(comp, dt);
	if (pObject)
	{
		return pObject;
	}

	//Set FaceDirection
	if (comp->GetVelocity() != FVector::ZeroVector)
		comp->SetFacingDirection(FVector(comp->GetVelocity().GetSafeNormal().X, comp->GetVelocity().GetSafeNormal().Y, 0.0f));

	m_IsAxisInput = false;
	m_PrevAxis = m_CurrAxis;
	return nullptr;
}

void UFSM_PCM_Run::SetPosDirection(FVector dir)
{
	m_PosDir = dir;
}

void UFSM_PCM_Run::Exit(UPlayerCharacterMovementComponent* comp)
{

}

UObject* UFSM_PCM_Run::HandleLeftRight(UPlayerCharacterMovementComponent* comp, float axis)
{
	//Get data
	FVector vel = comp->GetVelocity();
	float speed = vel.Size();
	float maxAcc = comp->GetMaxAccelerationXY();
	float maxSpeed = comp->GetMaxSpeedXY();

	FVector newVel = m_PosDir * speed;

	float sign = FVector::DotProduct(m_PosDir, vel.GetSafeNormal());
	if (sign < 0.0f)
		newVel *= (-1.0f);

	//Get delta time
	UWorld* pWorld = comp->GetWorld();
	if (!pWorld) return nullptr;
	float dt = pWorld->GetDeltaSeconds();

	//Get positive direction and multiply it with axis and acc..
	newVel += m_PosDir * maxAcc * axis * dt;
	//newVel = newVel.GetClampedToMaxSize(maxSpeed);

	float axisDiff = FMath::Abs(m_CurrAxis - m_PrevAxis);
	float minAxisDiff = 0.01;
	((axisDiff < minAxisDiff) && m_IsAxisInput) ? newVel = newVel.GetClampedToMaxSize(maxSpeed * FMath::Abs(axis)) : newVel = newVel.GetClampedToMaxSize(maxSpeed);

	FHitResult Hit{};
	if (!comp->AddWorldOffset(newVel * dt, Hit))
	{
		AActor* pOwner = comp->GetOwner();
		if (!pOwner) return nullptr;

		//Change direction to the new slope angle if slope is not too steep
		float angle = HelpFunc::CalculateSlopeAngleDegrees(Hit.ImpactNormal);
		float maxAngle = comp->GetMaxSlopeAngle();
		if (angle > maxAngle)//Slope is too steep
		{
			//UE_LOG(LogTemp, Warning, TEXT("Box Location: %s"), *pOwner->GetActorLocation().ToString());
			//UE_LOG(LogTemp, Warning, TEXT("Impact Point: %s"), *Hit.ImpactPoint.ToString());
			//UE_LOG(LogTemp, Warning, TEXT("Box Z value: %f"), pOwner->FindComponentByClass<UBoxComponent>()->GetScaledBoxExtent().Z);

			float heightDifference = Hit.ImpactPoint.Z - (pOwner->GetActorLocation().Z - pOwner->FindComponentByClass<UBoxComponent>()->GetScaledBoxExtent().Z);
			//UE_LOG(LogTemp, Warning, TEXT("height difference: %f"), heightDifference);


			if (FMath::Abs(heightDifference) < comp->GetRunHeightTolerance())
			{
				FHitResult hitResult{};
				UBoxComponent* pBox = pOwner->FindComponentByClass<UBoxComponent>();
				float heightTolerance = comp->GetRunHeightTolerance();

				bool isHit{};
				FCollisionQueryParams params{};
				params.bTraceComplex = false;
				params.AddIgnoredActor(pOwner);
				FVector boxScaledExtent = pBox->GetScaledBoxExtent();
				boxScaledExtent.Z -= heightTolerance / 2.0f;

				isHit = pWorld->SweepSingleByChannel
				(
					hitResult,
					pOwner->GetActorLocation() + FVector(0.0f, 0.0f, heightTolerance / 2.0f + 0.1f),
					pOwner->GetActorLocation() + FVector(0.0f, 0.0f, heightTolerance / 2.0f + 0.1f) + newVel.GetSafeNormal() * 5.0f,
					pOwner->GetActorRotation().Quaternion(),
					ECollisionChannel::ECC_Visibility,
					FCollisionShape::MakeBox(boxScaledExtent),
					params
				);

				UE_LOG(LogTemp, Warning, TEXT("Height Difference: %f"), heightDifference);

				if (!isHit)
				{
					UE_LOG(LogTemp, Warning, TEXT("STEP OVER!"))
					FVector movDir = newVel.GetSafeNormal();
					pOwner->AddActorWorldOffset(FVector(movDir.X * 0.2f, movDir.Y * 0.2f, comp->GetRunHeightTolerance()));
					comp->SetVelocity(newVel);
					return nullptr;
				}

				UE_LOG(LogTemp, Warning, TEXT("Box Extent: %s"), *boxScaledExtent.ToString())
			}

			comp->StopMovement();
			return nullptr;
		}


		FVector cross = FVector::CrossProduct(comp->GetPositiveYAxis(), Hit.ImpactNormal);
		m_PosDir = cross; //Set direction along the new detected slope
		return nullptr;
	}


	comp->SetVelocity(newVel);
	return nullptr;
}

UObject* UFSM_PCM_Run::DampActor(UPlayerCharacterMovementComponent* comp, float dt)
{
	if (m_IsAxisInput)
	{
		return nullptr;
	}

	//If speed is under a certain treshold, stop the movement completely
	float minSpeed{ 0.1f };
	if (comp->GetVelocity().Size() < minSpeed)
	{
		return TransitionToIdle(comp);
	}

	//Get data
	FVector vel = comp->GetVelocity();
	float speed = vel.Size();
	float maxSpeed = comp->GetMaxSpeedXY();
	float damp = comp->GetRunDampingXY();

	//Get velocity and take the opposite direction. Add the world offset
	FVector offset = vel.GetSafeNormal();
	offset *= (-1.0f) * damp * dt;
	offset = offset.GetClampedToSize(0.0f, vel.Size() * 0.9); // This way, the offset will never go to the opposite side 

	FVector diff = vel + offset;

	if (comp->AddWorldOffset(offset * dt))
	{
		comp->SetVelocity(diff);
	}

	return  nullptr;
}

UObject* UFSM_PCM_Run::HandleJump(UPlayerCharacterMovementComponent* comp)
{
	UWorld* pWorld = comp->GetWorld();
	if (!pWorld) return nullptr;

	FVector vel = comp->GetVelocity();
	if (vel.Z < 0.0f) comp->SetVelocityZ(comp->GetJumpSpeed());
	else
	{
		comp->SetVelocityZ(comp->GetJumpSpeed());
		comp->AddWorldOffset(FVector(0.0f, 0.0f, comp->GetVelocity().Z * pWorld->GetDeltaSeconds()));
	}

	UFSM_PCM_Air* pAir = NewObject<UFSM_PCM_Air>();
	if (!pAir) return nullptr;
	pAir->SetNrJumpLeft(comp->GetMaxNrOfJumps() - 1);

	FVector faceDir = comp->GetFacingDirection();
	comp->SetFacingDirection(FVector(faceDir.X, faceDir.Y, 0.0f));

	comp->OnJump.Broadcast();

	return pAir;
}

UObject* UFSM_PCM_Run::HandleUpDown(UPlayerCharacterMovementComponent* comp, float dt)
{

	AActor* pOwner = comp->GetOwner();
	if (!pOwner) return nullptr;

	//If ground is hit, change state
	FHitResult hitResult{};
	if (IsGroundHitByRayTraceBox(comp, dt))
	{
		//UE_LOG(LogTemp, Warning, TEXT("slopeangle: %f"), slopeAngle);
		return nullptr;
	}

	//If not hit ground, check again if the next slope isn't too low
	float dropDistance{ 10.0f };
	if (IsGroundHitByRayTraceBox(comp, dt, dropDistance, hitResult))
	{
		comp->AddWorldOffset(FVector(0.0f, 0.0f, -dropDistance));

		FVector dir = FVector::CrossProduct(comp->GetPositiveYAxis(), hitResult.ImpactNormal);
		float maxAngle = comp->GetMaxSlopeAngle();
		float slopeAngle = HelpFunc::CalculateSlopeAngleDegrees(hitResult.ImpactNormal);

		if (slopeAngle > maxAngle)//Slope is too steep
		{
			//return TransitionToSlide(comp, dir);
			return nullptr;
		}

		//Set direction along the new new detected slope
		m_PosDir = dir;
		return nullptr;
	}

	//Transition to air if no ground is detected anymore
	return TransitionToFall(comp);
}

bool UFSM_PCM_Run::IsGroundHitByRayTraceBox(UPlayerCharacterMovementComponent* comp, float dt)
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

	//If ground is hit, change state
	float rayLength = 2.0f;

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
		pOwner->GetActorLocation() - FVector(0.0f, 0.0f, rayLength),
		pOwner->GetActorRotation().Quaternion(),
		ECollisionChannel::ECC_Visibility,
		FCollisionShape::MakeBox(pBox->GetScaledBoxExtent()),
		params
	);

	if (isHit)
	{
		m_IsTouchingGround = true;

		return true;
	}

	return false;
}

bool UFSM_PCM_Run::IsGroundHitByRayTraceBox(UPlayerCharacterMovementComponent* comp, float dt, float rayLength, FHitResult& outHitResult)
{
	//Check Actor
	AActor* pOwner = comp->GetOwner();
	if (!pOwner) return false;

	//Check Box
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
		pOwner->GetActorLocation() - FVector(0.0f, 0.0f, rayLength),
		pOwner->GetActorRotation().Quaternion(),
		ECollisionChannel::ECC_Visibility,
		FCollisionShape::MakeBox(pBox->GetScaledBoxExtent()),
		params
	);

	if (isHit)
	{
		outHitResult = hitResult;
		m_IsTouchingGround = true;

		return true;
	}

	return false;
}

bool UFSM_PCM_Run::IsGroundHitByRayTraceLine(UPlayerCharacterMovementComponent* comp, float dt)
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

	//If ground is hit, change state
	float rayLength = 10.0f;

	//Start Ray Trace with capsule shape
	FHitResult hitResult{};
	FCollisionQueryParams params{};
	params.bTraceComplex = false;
	params.AddIgnoredActor(pOwner);

	FVector startLocation = pOwner->GetActorLocation() - FVector(0.0f, 0.0f, pBox->GetScaledBoxExtent().Z / 2.0f);
	FVector endLocation = startLocation - FVector(0.0f, 0.0f, rayLength);

	bool isHit{};
	isHit = pWorld->LineTraceSingleByChannel
	(
		hitResult,
		startLocation,
		endLocation,
		ECollisionChannel::ECC_Visibility,
		params
	);

	if (isHit)
	{
		FVector cross = FVector::CrossProduct(comp->GetPositiveYAxis(), hitResult.ImpactNormal);

		//	m_PosDir = cross;	

		return true;
	}

	return false;
}

UObject* UFSM_PCM_Run::TransitionToSlide(UPlayerCharacterMovementComponent* comp, FVector newDir)
{
	UFSM_PCM_Slide* pSlide = NewObject<UFSM_PCM_Slide>();
	if (newDir.Z > 0.0f) newDir *= -1.0f; //Make sure the slide direction is pointing downwards
	pSlide->SetDirection(newDir);
	comp->SetFacingDirection(newDir);
	Exit(comp);

	comp->OnSlide.Broadcast();

	return pSlide;
}

UObject* UFSM_PCM_Run::TransitionToFall(UPlayerCharacterMovementComponent* comp)
{
	UFSM_PCM_Air* pAir = NewObject<UFSM_PCM_Air>();
	if (!pAir) return nullptr;
	comp->SetVelocityZ(0.0f);
	pAir->SetNrJumpLeft(comp->GetMaxNrOfJumps());
	Exit(comp);

	comp->OnFall.Broadcast();

	return pAir;
}

UObject* UFSM_PCM_Run::TransitionToIdle(UPlayerCharacterMovementComponent* comp)
{
	UFSM_PCM_Idle* pIdle = NewObject<UFSM_PCM_Idle>();
	if (!pIdle) return nullptr;
	comp->StopMovement();
	Exit(comp);
	pIdle->SetPosDirection(m_PosDir);
	comp->OnIdle.Broadcast();
	return pIdle;
}