// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FSM_PlayerCharacterMovement.h"
#include "FSM_PCM_Run.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UFSM_PCM_Run : public UObject, public IFSM_PlayerCharacterMovement
{
	GENERATED_BODY()

public:
	virtual void Initialize(UPlayerCharacterMovementComponent* comp) override;

	virtual UObject* HandleInput(UPlayerCharacterMovementComponent* comp, APlayerCharacterController::CharacterInput input, float axis = 0) override;

	virtual UObject* Tick(UPlayerCharacterMovementComponent* comp, float dt) override;

	void SetPosDirection(FVector dir);

	virtual void Exit(UPlayerCharacterMovementComponent* comp) override;

private:
	UObject* HandleLeftRight(UPlayerCharacterMovementComponent* comp, float axis = 0);

	UObject* DampActor(UPlayerCharacterMovementComponent* comp, float dt);

	UObject* HandleJump(UPlayerCharacterMovementComponent* comp);

	UObject* HandleUpDown(UPlayerCharacterMovementComponent* comp, float dt);

	bool IsGroundHitByRayTraceBox(UPlayerCharacterMovementComponent* comp, float dt);

	bool IsGroundHitByRayTraceBox(UPlayerCharacterMovementComponent* comp, float dt, float rayLength, FHitResult& outHitResult);

	bool IsGroundHitByRayTraceLine(UPlayerCharacterMovementComponent* comp, float dt);

	UObject* TransitionToSlide(UPlayerCharacterMovementComponent* comp, FVector newDir);

	UObject* TransitionToFall(UPlayerCharacterMovementComponent* comp);

	UObject* TransitionToIdle(UPlayerCharacterMovementComponent* comp);

private:
	bool m_IsAxisInput;
	bool m_IsTouchingGround{ true };
	FVector m_PosDir{};
	FVector m_NewVel{};

	float m_PrevAxis{};
	float m_CurrAxis{};
};
