// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterController.h"
#include "Components/InputComponent.h"
#include "GameFramework/Pawn.h"
#include "PlayerCharacter.h"
#include "Engine/World.h"
#include "MyGameInstance.h"
#include "PlayerSoul.h"
#include "PlayerCharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreaming.h"
#include "Engine/Level.h"
#include "FLoadDimensionLevelStream.h"
#include "GameProjectGameMode.h"

APlayerCharacterController::APlayerCharacterController()
{
	
}

void APlayerCharacterController::BeginPlay()
{
	Super::BeginPlay();

	bAutoManageActiveCameraTarget = false;
	//OnSpawnSoul.AddDynamic(this, &APlayerCharacterController::TestFunction);
	//if (!OnSpawnSoul.IsBound()) UE_LOG(LogTemp, Warning, TEXT("NOT BOUND"));
}

void APlayerCharacterController::SetupInputComponent()
{
	Super::SetupInputComponent();


	if (!InputComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayerCharacterController::SetupInputComponent: pInputComp is an invalid pointer!"));
		return;
	}

	InputComponent->BindAxis(FName("MoveLeftRight"), this, &APlayerCharacterController::MoveLeftRight);
	InputComponent->BindAxis(FName("MoveDownUp"), this, &APlayerCharacterController::MoveDownUp);
	InputComponent->BindAction(FName("Jump"), EInputEvent::IE_Pressed, this, &APlayerCharacterController::Jump);
	InputComponent->BindAction(FName("Possess"), EInputEvent::IE_Pressed, this, &APlayerCharacterController::Posses_ThrowSoul);
	InputComponent->BindAction(FName("SwitchDimension"), EInputEvent::IE_Pressed, this, &APlayerCharacterController::SwitchDimension);
}

bool APlayerCharacterController::GetIsDimension() const
{
	return m_bIsDimension;
}

void APlayerCharacterController::SetIsInPossessionMode(bool state)
{
	m_bIsInPossessionMode = state;
}

void APlayerCharacterController::Jump()
{
	APawn* pPawn = GetPawn();
	if (!pPawn) return;

	IPossessable* pPlayer = Cast<IPossessable>(pPawn);
	if (!pPlayer) return;

	pPlayer->HandleInput(APlayerCharacterController::CharacterInput::jump);
}

void APlayerCharacterController::MoveLeftRight(float axis)
{
	APawn* pPawn = GetPawn();
	if (!pPawn) return;

	IPossessable* pPlayer = Cast<IPossessable>(pPawn);
	if (!pPlayer) return;

	pPlayer->HandleInput(APlayerCharacterController::CharacterInput::moveLeftRight, axis);
}

void APlayerCharacterController::MoveDownUp(float axis)
{
	APawn* pPawn = GetPawn();
	if (!pPawn) return;

	IPossessable* pPlayer = Cast<IPossessable>(pPawn);
	if (!pPlayer) return;

	pPlayer->HandleInput(APlayerCharacterController::CharacterInput::moveDownUp, axis);
}

void APlayerCharacterController::Posses_ThrowSoul()
{
	if (m_bIsDimension)
	{
		APawn* pPawn = GetPawn();
		if (!pPawn) return;

		IPossessable* pPlayer = Cast<IPossessable>(pPawn);
		if (!pPlayer) return;

		pPlayer->HandleInput(APlayerCharacterController::CharacterInput::possess);
	}
}

void APlayerCharacterController::SwitchDimension()
{
	if (m_bIsInPossessionMode) return;

	ULevelStreaming *GetCurrentLevelStreaming = UGameplayStatics::GetStreamingLevel(GetWorld(), FName());
	const FLevelCollection *pLevelColl = GetWorld()->GetActiveLevelCollection();
	TArray<ULevel*> levels = pLevelColl->GetLevels().Array();

	if (!m_bIsDimension)
	{
		for (int32 idx{ 1 }; idx < levels.Num(); ++idx)
		{
			FString levelName = levels[idx]->GetFullGroupName(true);
			FString dimensionLevelName = levelName + TEXT("_Dimension");

			UGameplayStatics::LoadStreamLevel(GetWorld(), FName(*dimensionLevelName), true, false, FLatentActionInfo{});


			//FLatentActionInfo latentInfo{};
			//UGameplayStatics::UnloadStreamLevel(GetWorld(), FName(*levelName), latentInfo, false);

			//Start latent function to load dimension level after the normal level is unloaded
			//FLatentActionManager& LatentActionManager = GetWorld()->GetLatentActionManager();
			//if (LatentActionManager.FindExistingAction<FLoadDimensionLevelStream>(this, idx) == NULL)
			//{
			//	LatentActionManager.AddNewAction(this, 0, new FLoadDimensionLevelStream(FName(*levelName), FName(*dimensionLevelName), GetWorld()));
			//}

		}
		OnSwitchToSoulDimension.Broadcast();
		m_bIsDimension = !m_bIsDimension;
	}
	else
	{
		for (int32 idx{ 1 }; idx < levels.Num(); ++idx)
		{
			//FString dimensionLevelName = levels[idx]->GetFullGroupName(true);
			//int32 substrIdx = dimensionLevelName.Find(TEXT("_Dimension"));
			//FString levelName{dimensionLevelName.Left(substrIdx)};
			//UE_LOG(LogTemp, Warning, TEXT("%s"), *levelName);
			
			FString levelName = levels[idx]->GetFullGroupName(true);
			FString dimensionLevelName = levelName + TEXT("_Dimension");

			FLatentActionInfo latentInfo{};
			UGameplayStatics::UnloadStreamLevel(GetWorld(), FName(*dimensionLevelName), latentInfo, false);

			//Start latent function to load dimension level after the normal level is unloaded
			//FLatentActionManager& LatentActionManager = GetWorld()->GetLatentActionManager();
			//if (LatentActionManager.FindExistingAction<FLoadDimensionLevelStream>(this, idx) == NULL)
			//{
			//	LatentActionManager.AddNewAction(this, 0, new FLoadDimensionLevelStream(FName(*dimensionLevelName), FName(*levelName), GetWorld()));
			//}
		}
		DestroyPawnIfOverlapping();
		OnSwitchToNormalDimension.Broadcast();
		m_bIsDimension = !m_bIsDimension;

	}


}

void APlayerCharacterController::LoadDimensionLatentFunction(int32 a)
{

};

inline void APlayerCharacterController::DestroyPawnIfOverlapping()
{
	APawn* pControlledPawn = GetPawn();
	if (!pControlledPawn) return;
	APlayerCharacter* pPlayerCharacter = Cast<APlayerCharacter>(pControlledPawn);
	if (!pPlayerCharacter) return;

	if (pPlayerCharacter->GetIsOverlappingDimensionBox())
	{
		UE_LOG(LogTemp, Warning, TEXT("YOU DEAD LOL"));
		Cast<AGameProjectGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->StartGameOver();
		pPlayerCharacter->EnableInput(false);
		pPlayerCharacter->GetCrushedByDimensionMesh();
	}
}