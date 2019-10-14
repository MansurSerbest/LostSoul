// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "ConstructorHelpers.h"
#include "PlayerSoul.h"

TSubclassOf<APlayerSoul> UMyGameInstance::PlayerSoulClass{};

UMyGameInstance:: UMyGameInstance()
{
	UE_LOG(LogTemp, Warning, TEXT("UMyGameInstance:: UMyGameInstance"));

	//Init playerSoul blueprint class
	ConstructorHelpers::FClassFinder<APlayerSoul> playerSoulFinder(TEXT("/Game/Character/BP_PlayerSoul"));
	if (playerSoulFinder.Succeeded())
	{
		PlayerSoulClass = playerSoulFinder.Class;
		UE_LOG(LogTemp, Warning, TEXT("PlayerSoul Class: %s"), *PlayerSoulClass.Get()->GetName());
	}
	else UE_LOG(LogTemp, Warning, TEXT("UMyGameInstance:: UMyGameInstance: playerSoulFinder did not succeed!"));

}
