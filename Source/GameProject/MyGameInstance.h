// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PlayerSoul.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
class APlayerSoul;

UCLASS()
class GAMEPROJECT_API UMyGameInstance : public UGameInstance
{

	GENERATED_BODY()

public:
	UMyGameInstance();
	
	static TSubclassOf<APlayerSoul> PlayerSoulClass;
};
