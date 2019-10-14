// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerCharacterController.h"
#include "FSM_PlayerCharacterMovement.generated.h"

class UPlayerCharacterMovementComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFSM_PlayerCharacterMovement : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GAMEPROJECT_API IFSM_PlayerCharacterMovement
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Initialize(UPlayerCharacterMovementComponent* comp) = 0;

	virtual UObject* HandleInput(UPlayerCharacterMovementComponent* comp, APlayerCharacterController::CharacterInput input, float axis = 0) = 0;

	virtual UObject* Tick(UPlayerCharacterMovementComponent* comp, float dt) = 0;

protected:
	virtual void Exit(UPlayerCharacterMovementComponent* comp) = 0;
};
