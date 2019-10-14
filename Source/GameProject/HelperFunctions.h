// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

class APlayerController;

namespace HelpFunc
{
	float CalcAngleDegrees(const FVector& a, const FVector& b);

	float CalcAngleRad(const FVector& a, const FVector& b);

	float CalculateSlopeAngleDegrees(const FVector& slopeNormal);

	float CalculateSlopeAngleRadians(const FVector& slopeNormal);

	FVector CalculateSlopeDirection(const FVector& slopeNormal, const FVector& rightVector);

	float GetGamePadLeftXDeadZone(APlayerController* pController);
}
