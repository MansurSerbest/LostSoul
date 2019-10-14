// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM_PCM_Idle.h"
#include "PlayerCharacterMovementComponent.h"
#include "Engine/World.h"
#include "FSM_PCM_Air.h"
#include "FSM_PCM_Run.h"
#include "Components/BoxComponent.h"

void UFSM_PCM_Idle::Initialize(UPlayerCharacterMovementComponent* comp)
{
	UE_LOG(LogTemp, Warning, TEXT("[%f]UFSM_PCM_Idle::Initialize"), comp->GetWorld()->GetTimeSeconds());

	if (m_PosDir == FVector::ZeroVector) m_PosDir = comp->GetPosMoveDirection();


}

UObject* UFSM_PCM_Idle::HandleInput(UPlayerCharacterMovementComponent* comp, APlayerCharacterController::CharacterInput input, float axis)
{
	switch (input)
	{
	case APlayerCharacterController::CharacterInput::moveLeftRight:
	{
		float newAxis{};
		float deadZone = comp->GetDeadZoneLeftX();
		if (FMath::Abs(axis) < deadZone)
			newAxis = 0.0f;
		else
			newAxis = (FMath::Abs(axis) - deadZone) / (1.0f - deadZone) * FMath::Sign(axis);
		return HandleLeftRight(comp, newAxis);
		break;
	}
	case APlayerCharacterController::CharacterInput::jump:
		return HandleJump(comp);
		break;
	case APlayerCharacterController::CharacterInput::possess:
		break;
	default:
		break;
	}

	return nullptr;
}

UObject* UFSM_PCM_Idle::Tick(UPlayerCharacterMovementComponent* comp, float dt)
{
	//If ground is hit, change state
	if (!IsGroundHitByRayTraceBox(comp, dt))
	{
		return TransitionToFall(comp);
	}

	return nullptr;
}

void UFSM_PCM_Idle::Exit(UPlayerCharacterMovementComponent* comp)
{

}

void UFSM_PCM_Idle::SetPosDirection(const FVector& dir)
{
	m_PosDir = dir;
}

UObject* UFSM_PCM_Idle::HandleJump(UPlayerCharacterMovementComponent* comp)
{
	UWorld* pWorld = comp->GetWorld();
	if (!pWorld) return nullptr;

	comp->SetVelocityZ(comp->GetJumpSpeed());
	comp->AddWorldOffset(FVector(0.0f, 0.0f, comp->GetVelocity().Z * pWorld->GetDeltaSeconds()));

	UFSM_PCM_Air* pAir = NewObject<UFSM_PCM_Air>();
	if (!pAir) return nullptr;
	pAir->SetNrJumpLeft(comp->GetMaxNrOfJumps() - 1);

	FVector faceDir = comp->GetFacingDirection();
	comp->SetFacingDirection(FVector(faceDir.X, faceDir.Y, 0.0f));

	comp->OnJump.Broadcast();

	return pAir;
}

UObject* UFSM_PCM_Idle::HandleLeftRight(UPlayerCharacterMovementComponent* comp, float axis)
{
	if (FMath::Abs(axis) > 0.0f)
	{
		UWorld* pWorld = comp->GetWorld();
		if (!pWorld) return nullptr;
		float dt = pWorld->GetDeltaSeconds();

		UFSM_PCM_Run* pRun = NewObject<UFSM_PCM_Run>();
		if (!pRun) return nullptr;

		// You need to set velocity here, or else, the update of run state will immediately transition to idle because velocity is zero
		FVector speed = m_PosDir * comp->GetMaxAccelerationXY() * axis * dt;
		speed = speed.GetClampedToMaxSize(comp->GetMaxSpeedXY());

		comp->SetVelocity(speed);
		pRun->SetPosDirection(m_PosDir);
		comp->OnRun.Broadcast();

		return pRun;
	}

	return nullptr;
}

UObject* UFSM_PCM_Idle::TransitionToRun(UPlayerCharacterMovementComponent* comp)
{
	return nullptr;
}

UObject* UFSM_PCM_Idle::TransitionToFall(UPlayerCharacterMovementComponent* comp)
{
	UFSM_PCM_Air* pAir = NewObject<UFSM_PCM_Air>();
	if (!pAir) return nullptr;
	comp->SetVelocityZ(0.0f);
	pAir->SetNrJumpLeft(comp->GetMaxNrOfJumps());
	Exit(comp);

	comp->OnFall.Broadcast();

	return pAir;
}

bool UFSM_PCM_Idle::IsGroundHitByRayTraceBox(UPlayerCharacterMovementComponent* comp, float dt)
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

	return isHit;
}