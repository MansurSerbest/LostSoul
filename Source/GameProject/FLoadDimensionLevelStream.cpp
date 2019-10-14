// Fill out your copyright notice in the Description page of Project Settings.


#include "FLoadDimensionLevelStream.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreaming.h"
#include "Engine/World.h"

FLoadDimensionLevelStream::FLoadDimensionLevelStream(const FName& levelToUnload, const FName& levelToLoad, UWorld *pWorld)
	:m_LevelToUnload{ levelToUnload }, m_LevelToLoad{ levelToLoad }, m_pWorld{pWorld}
{
}

FLoadDimensionLevelStream::~FLoadDimensionLevelStream()
{
}

// Return true when the action is completed
void FLoadDimensionLevelStream::UpdateOperation(FLatentResponse& Response)
{
	bool isUnloaded = UGameplayStatics::GetStreamingLevel(m_pWorld, m_LevelToUnload)->GetCurrentState() == ULevelStreaming::ECurrentState::Unloaded;

	if (!isUnloaded)
	{
		UE_LOG(LogTemp, Warning, TEXT("Still Unloading!"));
		return;
	}


	UGameplayStatics::LoadStreamLevel(m_pWorld, m_LevelToLoad, true, false, FLatentActionInfo{});
	Response.DoneIf(true);
}

void FLoadDimensionLevelStream::NotifyObjectDestroyed()
{

}

void FLoadDimensionLevelStream::NotifyActionAborted()
{

}
