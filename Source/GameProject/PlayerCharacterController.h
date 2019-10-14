// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharacterController.generated.h"

/**
 * 
 */

class APlayerSoul;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpawnSoul, APlayerSoul*, pSoul);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchToSoulDimension);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchToNormalDimension);


UCLASS()
class GAMEPROJECT_API APlayerCharacterController : public APlayerController
{
	GENERATED_BODY()

public:
	APlayerCharacterController();

	virtual void BeginPlay() override;

	bool GetIsDimension() const;

	void SetIsInPossessionMode(bool state);
protected:
	void SetupInputComponent() override;

private:
	void Jump();
	void MoveLeftRight(float axis);
	void MoveDownUp(float axis);
	void Posses_ThrowSoul();
	void SwitchDimension();

	UFUNCTION(meta = (Latent))
	void LoadDimensionLatentFunction(int32 a = 10);

	inline void DestroyPawnIfOverlapping();

public:
	enum class CharacterInput
	{
		moveLeftRight = 0,
		moveDownUp = 1,
		jump = 2,
		fire = 3,
		possess = 4
	};

	UPROPERTY(BlueprintAssignable)
	FOnSpawnSoul OnSpawnSoul;

	UPROPERTY(BlueprintAssignable)
	FOnSwitchToSoulDimension OnSwitchToSoulDimension;

	UPROPERTY(BlueprintAssignable)
	FOnSwitchToNormalDimension OnSwitchToNormalDimension;

private:
	bool m_bIsDimension{ false };
	bool m_bIsInPossessionMode{ false };
};
