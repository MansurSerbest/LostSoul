// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Possessable.h"
#include "PlayerCharacterController.h"
#include "PlayerSoul.generated.h"

class USphereComponent;
class APlayerCharacter;
class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class GAMEPROJECT_API APlayerSoul : public APawn, public IPossessable
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerSoul();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetMoveDirection(FVector dir);

	void GetPossessed(APlayerSoul* pSoul) override;

	void HandleInput(APlayerCharacterController::CharacterInput input, float axis = 0) override;

	void SetSoulOwner(APlayerCharacter* pPlayer);

	APlayerCharacter* GetSoulOwner() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void HandleLeftRight(float axis);

	void DampActor(float dt);

	void HandleDownUp(float axis);

	void HandlePosses();

	bool IsPossessableDetected(TArray<AActor*>& overlappingActors);

	UFUNCTION()
		void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditDefaultsOnly, Category = Speed, meta =( DisplayName = MaxAcceleration))
		float m_MaxAcceleration;

	UPROPERTY(EditDefaultsOnly, Category = Speed, meta = (DisplayName = MaxSpeed))
		float m_MaxSpeed;

	UPROPERTY(EditDefaultsOnly, Category = Speed, meta = (DisplayName = Damping))
		float m_Damping;

	FVector m_Velocity;
	
	FVector m_MoveDirection;

	UPROPERTY(VisibleDefaultsOnly)
		USphereComponent* RadarSphereComponent;

	UPROPERTY(VisibleDefaultsOnly)
		UStaticMeshComponent *StaticMeshComponent;

	UPROPERTY(VisibleDefaultsOnly)
		USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleDefaultsOnly)
		UCameraComponent* CameraComponent;

	APlayerCharacter* m_pOwner;

	bool m_IsHorAxisInput{ false };
	bool m_IsVertAxisInput{ false };

	FVector m_SpawnLocation{};

	UPROPERTY(EditAnywhere, Category = MoveRadius, meta = (DisplayName = "MoveRadius"))
		float m_MoveRadius = 500.0f;

};
