// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LatentActions.h"

/**
 * 
 */

class UWorld;

class GAMEPROJECT_API FLoadDimensionLevelStream : public FPendingLatentAction
{
public:
	FLoadDimensionLevelStream(const FName& levelToUnload, const FName& levelToLoad, UWorld* pWorld);
	~FLoadDimensionLevelStream();

	// Return true when the action is completed
	virtual void UpdateOperation(FLatentResponse& Response) override;

	// Lets the latent action know that the object which originated it has been garbage collected
	// and the action is going to be destroyed (no more UpdateOperation calls will occur and
	// CallbackTarget is already NULL)
	// This is only called when the object goes away before the action is finished; perform normal
	// cleanup when responding that the action is completed in UpdateOperation
	virtual void NotifyObjectDestroyed() override;

	virtual void NotifyActionAborted() override;

private:
	float m_CurrTimer{ 0.0f };
	float m_MaxTimer{ 5.0f };
	FName m_LevelToUnload{};
	FName m_LevelToLoad{};
	UWorld* m_pWorld;
};
