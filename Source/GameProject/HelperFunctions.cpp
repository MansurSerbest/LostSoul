// Fill out your copyright notice in the Description page of Project Settings.


#include "HelperFunctions.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/PlayerController.h"


float HelpFunc::CalcAngleDegrees(const FVector& a, const FVector& b)
{
	FVector aNor = a.GetSafeNormal();
	FVector bNor = b.GetSafeNormal();
	float dot = FVector::DotProduct(aNor, bNor);
	float angle = FMath::RadiansToDegrees(FMath::Acos(dot));

	return angle;
}

float HelpFunc::CalcAngleRad(const FVector& a, const FVector& b)
{
	FVector aNor = a.GetSafeNormal();
	FVector bNor = b.GetSafeNormal();
	float dot = FVector::DotProduct(aNor, bNor);
	float angle = FMath::Acos(dot);

	return angle;
}

float HelpFunc::CalculateSlopeAngleDegrees(const FVector& slopeNormal)
{
	FVector baseDir = FVector(slopeNormal.X, slopeNormal.Y, 0.0f);
	float slopeAngle = HelpFunc::CalcAngleDegrees(slopeNormal, baseDir);
	slopeAngle = 90.0f - slopeAngle; //Draw on paper, you'll understand why I did this

	return slopeAngle;
}

float HelpFunc::CalculateSlopeAngleRadians(const FVector& slopeNormal)
{
	FVector baseDir = FVector(slopeNormal.X, slopeNormal.Y, 0.0f);
	float slopeAngle = HelpFunc::CalcAngleRad(slopeNormal, baseDir);
	slopeAngle = (PI/2.0f) - slopeAngle; //Draw on paper, you'll understand why I did this

	return slopeAngle;
}

FVector HelpFunc::CalculateSlopeDirection(const FVector& slopeNormal, const FVector& rightVector)
{
	FVector cross = FVector::CrossProduct(rightVector, slopeNormal);
	cross = cross.GetSafeNormal();

	return cross;
}

float HelpFunc::GetGamePadLeftXDeadZone(APlayerController* pController)
{
	if (!pController) return 0.0f;

	FInputAxisProperties prop{};

	if (pController->PlayerInput->GetAxisProperties(FKey(FName("Gamepad_LeftX")), prop))
	{
		return prop.DeadZone;
	}

	return 0.0f;
}