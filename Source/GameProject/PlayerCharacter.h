// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ScriptInterface.h"
#include "PlayerCharacterController.h"
#include "Possessable.h"
#include "PlayerCharacter.generated.h"

class UPlayerCharacterMovementComponent;
class USkeletalMeshComponent;
class UBoxComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class GAMEPROJECT_API APlayerCharacter : public APawn, public IPossessable
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void HandleInput(APlayerCharacterController::CharacterInput input, float axis = 0) override;

	void GetPossessed(APlayerSoul* pSoul) override;

	bool GetIsOverlappingDimensionBox() const;

	void GetCrushedByDimensionMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void HandlePossess();

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	UPROPERTY(VisibleAnywhere, meta = (DisplayName = "MovementComponent"))
		UPlayerCharacterMovementComponent* MovementComponent;

	UPROPERTY(VisibleDefaultsOnly, meta = (DisplayName = "PlatformDetector"))
		UBoxComponent *PlatformDetector;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "SkeletalMesh"))
		USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleDefaultsOnly, meta = (DisplayName = "SpringArmComponent"))
		USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleDefaultsOnly, meta = (DisplayName = "CameraComponent"))
		UCameraComponent* CameraComponent;

private:
	bool m_IsOverlappingDimensionBox{ false };
};
