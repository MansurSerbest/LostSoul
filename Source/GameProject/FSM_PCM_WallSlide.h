// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FSM_PlayerCharacterMovement.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FSM_PCM_WallSlide.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UFSM_PCM_WallSlide : public UObject, public IFSM_PlayerCharacterMovement
{
	GENERATED_BODY()

public:
	virtual void Initialize(UPlayerCharacterMovementComponent* comp) override;

	virtual UObject* HandleInput(UPlayerCharacterMovementComponent* comp, APlayerCharacterController::CharacterInput input, float axis = 0) override;

	virtual UObject* Tick(UPlayerCharacterMovementComponent* comp, float dt) override;

	void SetWallDirection(const FVector& dir);

protected:
	virtual void Exit(UPlayerCharacterMovementComponent* comp) override;

private:
	UObject* HandleJump(UPlayerCharacterMovementComponent* comp);

	bool IsWallDetected(UPlayerCharacterMovementComponent* comp, float dt);

	UObject* HandleSlideDown(UPlayerCharacterMovementComponent* comp, float dt);

	UObject* TransitionToRun(UPlayerCharacterMovementComponent* comp, FVector newDir);

	UObject* TransitionToFall(UPlayerCharacterMovementComponent* comp);

	UObject* TransitionToJump(UPlayerCharacterMovementComponent* comp);

	bool IsWallHitByRayTraceBox(UPlayerCharacterMovementComponent* comp, float dt, float rayLength);

private:
	FVector m_WallDir{};

	FVector m_GroundDirection{};

	FVector m_SlideDirection{};

};
