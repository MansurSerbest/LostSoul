// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM_PCM_Slide.h"
#include "PlayerCharacterMovementComponent.h"
#include "Engine/World.h"
#include "HelperFunctions.h"
#include "GameFramework/Actor.h"
#include "FSM_PCM_Run.h"

void UFSM_PCM_Slide::Initialize(UPlayerCharacterMovementComponent* comp)
{
	UE_LOG(LogTemp, Warning, TEXT("[%f]UFSM_PCM_Slide::Initialize"), comp->GetWorld()->GetTimeSeconds());

}

UObject* UFSM_PCM_Slide::HandleInput(UPlayerCharacterMovementComponent* comp, APlayerCharacterController::CharacterInput input, float axis)
{
	switch (input)
	{
	case APlayerCharacterController::CharacterInput::moveLeftRight:
		return nullptr;
		break;
	case APlayerCharacterController::CharacterInput::jump:
		return nullptr;
		break;
	default:
		return nullptr;
	}
}

UObject* UFSM_PCM_Slide::Tick(UPlayerCharacterMovementComponent* comp, float dt)
{
	UObject* pObject = HandleSlide(comp, dt);
	if (pObject) return pObject;

	return nullptr;
}

UObject* UFSM_PCM_Slide::HandleSlide(UPlayerCharacterMovementComponent* comp, float dt)
{
	//Get data
	float maxAcc = comp->GetMaxAccelerationXY();
	float maxSpeed = comp->GetMaxSpeedXY();
	FVector vel = m_Dir * m_SlideSpeed;

	//If ground is hit, change state
	FHitResult hitResult{};
	if (!comp->AddWorldOffset(vel * dt, hitResult))
	{
		AActor* pOwner = comp->GetOwner();
		if (!pOwner) return nullptr;

		float slopeAngle = HelpFunc::CalculateSlopeAngleDegrees(hitResult.ImpactNormal);
		FVector dir = FVector::CrossProduct(pOwner->GetActorRightVector(), hitResult.ImpactNormal);

		//If to steep, keep sliding, but change direction
		if (slopeAngle > comp->GetMaxSlopeAngle())
		{
			m_Dir = dir;
			return nullptr;
		}

		//If not to steep, transition to Run	
		return TransitionToRun(comp, dir);
	}

	return nullptr;
}

UObject* UFSM_PCM_Slide::TransitionToRun(UPlayerCharacterMovementComponent* comp, const FVector& dir)
{
	comp->StopMovementZ();
	UFSM_PCM_Run* pRun = NewObject<UFSM_PCM_Run>();
	if (pRun) pRun->SetPosDirection(dir);
	Exit(comp);

	comp->OnRun.Broadcast();

	return pRun;
}

void UFSM_PCM_Slide::SetDirection(FVector dir)
{
	m_Dir = dir;
}

void UFSM_PCM_Slide::Exit(UPlayerCharacterMovementComponent* comp)
{

}