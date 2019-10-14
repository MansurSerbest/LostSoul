// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerCharacterController.h"
#include "Possessable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPossessable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */

class APlayerCharacter;
class APlayerSoul;

class GAMEPROJECT_API IPossessable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void GetPossessed(APlayerSoul* pSoul) = 0;

	virtual void HandleInput(APlayerCharacterController::CharacterInput input, float axis = 0) = 0;

};
