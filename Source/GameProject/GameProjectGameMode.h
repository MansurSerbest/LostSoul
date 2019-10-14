// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameProjectGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API AGameProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void StartGameOver();
private:
	bool m_IsGameOver{ false };
};
