// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCharacterController.h"
#include "PlayerCharacterMovementComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIdle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRun);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJump);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFall);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSlide);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWallSlide);

class IFSM_PlayerCharacterMovement;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GAMEPROJECT_API UPlayerCharacterMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlayerCharacterMovementComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Getters and Setter
	FVector GetVelocity() const;

	FVector GetVelocityXY() const;

	float GetMaxSpeedXY() const;

	float GetSpeedXY() const;

	float GetSpeedZ() const;

	float GetMaxAccelerationXY() const;

	float GetRunDampingXY() const;

	float GetAirDampingXY() const;

	float GetJumpSpeed() const;

	float GetGravity() const;

	void HandleInput(APlayerCharacterController::CharacterInput input, float axis);

	bool AddWorldOffset(FVector offset, float minTreshold = 0.1f);

	bool AddWorldOffset(FVector offset, FHitResult& outHit, float minTreshold = 0.1f);

	void SetVelocityXY(FVector newVel);

	void SetVelocityZ(float newVel);

	void SetVelocity(FVector newVel);

	FVector GetForwardDirection();

	FVector GetPosMoveDirection() const;

	FVector GetFacingDirection() const;

	void SetFacingDirection(const FVector& vec);

	void StopMovementXY();

	void StopMovementZ();

	void StopMovement();

	float GetMaxSlopeAngle() const;

	int32 GetMaxNrOfJumps() const;

	float GetWallSlideMaxSpeed() const;

	float GetWallSlideAcceleration() const;

	float GetWallHorJumpSpeed() const;

	float GetWallVertJumpSpeed() const;

	float GetDeadZoneLeftX() const;

	FVector GetPositiveYAxis() const;

	float GetRunHeightTolerance() const;

	float GetLandingVelocityRatio() const;

	UFUNCTION(BlueprintCallable)
	void SetIsOnMovingPlatform(bool b);

	UFUNCTION(BlueprintCallable)
	bool GetIsOnMovingPlatform() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	void InitFSM();

	void HandleTickFSM(float DeltaTime);

	void ChangeState(UObject* pObject);

public:
	UPROPERTY(BlueprintAssignable)
		FOnIdle OnIdle;

	UPROPERTY(BlueprintAssignable)
		FOnRun OnRun;

	UPROPERTY(BlueprintAssignable)
		FOnJump OnJump;

	UPROPERTY(BlueprintAssignable)
		FOnFall OnFall;

	UPROPERTY(BlueprintAssignable)
		FOnSlide OnSlide;

	UPROPERTY(BlueprintAssignable)
		FOnWallSlide OnWallSlide;

private:
	UPROPERTY(VisibleAnywhere, Category = Velocity, meta = (DisplayName = "Velocity"))
		FVector m_Velocity;

	UPROPERTY(EditDefaultsOnly, Category = Velocity, meta = (DisplayName = "MaxSpeed"))
		float m_MaxSpeedXY{ 800.0f };

	UPROPERTY(EditDefaultsOnly, Category = Acceleration, meta = (DisplayName = "MaxAcceleration"))
		float m_MaxAccelerationXY{ 1600.0f };

	UPROPERTY(EditDefaultsOnly, Category = Acceleration, meta = (DisplayName = "RunDamping"))
		float m_RunDampingXY{ 2000.0f };

	UPROPERTY(EditDefaultsOnly, Category = Acceleration, meta = (DisplayName = "AirDamping"))
		float m_AirDampingXY{ 2000.0f };

	UPROPERTY(EditDefaultsOnly, Category = Acceleration, meta = (DisplayName = "Gravity"))
		float m_Gravity{ 1000.0f };

	UPROPERTY(EditDefaultsOnly, Category = Jump, meta = (DisplayName = "JumpSpeed"))
		float m_JumpSpeed{ 800.0f };

	UPROPERTY(EditDefaultsOnly, Category = Velocity, meta = (DisplayName = "MaxSlopeAngle"))
		float m_MaxSlopeAngle{ 40.0f };

	UPROPERTY(EditDefaultsOnly, Category = Jump, meta = (DisplayName = "MaxJumpNumber"))
		int32 m_MaxJumpNr {2};

	UPROPERTY(EditDefaultsOnly, Category = Run, meta = (DisplayName = "RunHeightTolerance"))
		float m_RunHeightTolerance{ 2.0f };

	UPROPERTY(EditDefaultsOnly, Category = Wall, meta = (DisplayName = "WallSlideMaxSpeed"))
		float m_WallSlideMaxSpeed{ 800.0f };

	UPROPERTY(EditDefaultsOnly, Category = Wall, meta = (DisplayName = "WallSlideAcceleration"))
		float m_WallSlideAcceleration{ 800.0f };

	UPROPERTY(EditDefaultsOnly, Category = Wall, meta = (DisplayName = "WallHorizontalJumpSpeed"))
		float m_WallHorJumpSpeed{ 800.0f };

	UPROPERTY(EditDefaultsOnly, Category = Wall, meta = (DisplayName = "WallVerticalJumpSpeed"))
		float m_WallVertJumpSpeed{ 800.0f };

	UPROPERTY(EditDefaultsOnly, Category = Run, meta = (DisplayName = "LandVelocityRatio"))
		float m_LandingVelocityRatio{ 1.0f };


	FVector m_MoveDirection;
	FVector m_FacingDirection;
	FVector m_PosMoveDirection;

	UPROPERTY()
		TScriptInterface<IFSM_PlayerCharacterMovement> m_pState;

	float m_DeadZoneLeftX{};

	bool m_IsOnMovingPlatform{ false };
};
