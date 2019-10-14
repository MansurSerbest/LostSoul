// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FSM_PlayerCharacterMovement.h"
#include "FSM_PCM_Air.generated.h"

/**
 * 
 */
class UFSM_PCM_Slide;
class UFSM_PCM_Run;

UCLASS()
class GAMEPROJECT_API UFSM_PCM_Air : public UObject, public IFSM_PlayerCharacterMovement
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(UPlayerCharacterMovementComponent* comp) override;

	virtual UObject* HandleInput(UPlayerCharacterMovementComponent* comp, APlayerCharacterController::CharacterInput input, float axis = 0) override;

	virtual UObject* Tick(UPlayerCharacterMovementComponent* comp, float dt) override;

	void SetNrJumpLeft(int32 nr);

protected:
	virtual void Exit(UPlayerCharacterMovementComponent* comp) override;

private:
	UObject* HandleLeftRight(UPlayerCharacterMovementComponent* comp, float axis = 0);

	void DampActor(UPlayerCharacterMovementComponent* comp, float dt);

	UObject* HandleUpDown(UPlayerCharacterMovementComponent* comp, float dt);

	UObject* HandleJump(UPlayerCharacterMovementComponent* comp, float axis = 0);

	UFUNCTION()
	void HitGround(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UObject* TransitionToSlide(UPlayerCharacterMovementComponent* comp, FVector newDir);

	UObject* TransitionToRun(UPlayerCharacterMovementComponent* comp);

	UObject* TransitionToIdle(UPlayerCharacterMovementComponent* comp);

	UObject* TransitionToWallSlide(UPlayerCharacterMovementComponent* comp, const FVector& wallDir);

private:
	bool m_IsAxisInput{ false };
	FVector m_GroundDirection{};
	int32 m_NrJumpLeft{};

};
