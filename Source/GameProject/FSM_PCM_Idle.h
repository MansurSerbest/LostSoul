// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FSM_PlayerCharacterMovement.h"
#include "FSM_PCM_Idle.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UFSM_PCM_Idle : public UObject, public IFSM_PlayerCharacterMovement
{
	GENERATED_BODY()

public:
	virtual void Initialize(UPlayerCharacterMovementComponent* comp) override;

	virtual UObject* HandleInput(UPlayerCharacterMovementComponent* comp, APlayerCharacterController::CharacterInput input, float axis = 0) override;

	virtual UObject* Tick(UPlayerCharacterMovementComponent* comp, float dt) override;

	virtual void Exit(UPlayerCharacterMovementComponent* comp) override;

	void SetPosDirection(const FVector& dir);

private:
	UObject* HandleJump(UPlayerCharacterMovementComponent* comp);

	UObject* HandleLeftRight(UPlayerCharacterMovementComponent* comp, float axis = 0.0f);

	UObject* TransitionToRun(UPlayerCharacterMovementComponent* comp);	

	UObject* TransitionToFall(UPlayerCharacterMovementComponent* comp);

	bool IsGroundHitByRayTraceBox(UPlayerCharacterMovementComponent* comp, float dt);

private:
	FVector m_PosDir{};
};
