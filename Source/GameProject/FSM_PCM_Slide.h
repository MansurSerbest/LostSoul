// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FSM_PlayerCharacterMovement.h"
#include "FSM_PCM_Slide.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UFSM_PCM_Slide : public UObject, public IFSM_PlayerCharacterMovement
{
	GENERATED_BODY()

public:
	virtual void Initialize(UPlayerCharacterMovementComponent* comp) override;

	virtual UObject* HandleInput(UPlayerCharacterMovementComponent* comp, APlayerCharacterController::CharacterInput input, float axis = 0) override;

	virtual UObject* Tick(UPlayerCharacterMovementComponent* comp, float dt) override;

	void SetDirection(FVector dir);
	
	virtual void Exit(UPlayerCharacterMovementComponent* comp) override;

private:
	UObject* HandleSlide(UPlayerCharacterMovementComponent* comp, float dt);

	UObject* TransitionToRun(UPlayerCharacterMovementComponent* comp, const FVector& dir);

private:
	FVector m_Dir{};
	float m_SlideSpeed{100.0f};
};
