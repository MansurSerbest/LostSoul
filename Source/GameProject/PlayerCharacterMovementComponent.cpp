// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterMovementComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "FSM_PlayerCharacterMovement.h"
#include "Object.h"
#include "FSM_PCM_Idle.h"
#include "FSM_PCM_Run.h"
#include "PlayerCharacter.h"
#include "GameFramework/Pawn.h"
#include "HelperFunctions.h"

// Sets default values for this component's properties
UPlayerCharacterMovementComponent::UPlayerCharacterMovementComponent()
	:m_Velocity{ 0.0f, 0.0f, 0.0f }, m_PosMoveDirection{ 1.0f, 0.0f, 0.0f }
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	//SetTickGroup(ETickingGroup::TG_EndPhysics);
}


// Called when the game starts
void UPlayerCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	InitFSM();

	//Set DeadZone Value to the engine setup value
	m_DeadZoneLeftX = HelpFunc::GetGamePadLeftXDeadZone(GetWorld()->GetFirstPlayerController());
	UE_LOG(LogTemp, Warning, TEXT("DeadZone: %f"), m_DeadZoneLeftX);
}

//Input
void UPlayerCharacterMovementComponent::HandleInput(APlayerCharacterController::CharacterInput input, float axis)
{
	UObject* pObject = m_pState->HandleInput(this, input, axis);
	if (pObject)
	{
		ChangeState(pObject);
	}

}

// Called every frame
void UPlayerCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HandleTickFSM(DeltaTime);
}

void UPlayerCharacterMovementComponent::InitFSM()
{
	UFSM_PCM_Run* pMove = NewObject<UFSM_PCM_Run>();
	if (!pMove)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerCharacterMovementComponent::InitFSM: pIdle is an invalid pointer!"));
		return;
	}

	ChangeState(pMove);
}



void UPlayerCharacterMovementComponent::HandleTickFSM(float DeltaTime)
{
	//If Tick return, it means the state needs to be changed
	UObject* pObject = m_pState->Tick(this, DeltaTime);
	if (pObject)
	{
		ChangeState(pObject);
	}


}

void UPlayerCharacterMovementComponent::ChangeState(UObject* pObject)
{
	if (!pObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerCharacterMovementComponent::InitFSM: pObject is an invalid pointer!"));
		return;
	}

	IFSM_PlayerCharacterMovement* pInterface = Cast<IFSM_PlayerCharacterMovement>(pObject);
	if (!pInterface)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerCharacterMovementComponent::InitFSM: pInterface is an invalid pointer!"));
		return;
	}

	//Initialize
	m_pState.SetInterface(pInterface);
	m_pState.SetObject(pObject);
	m_pState->Initialize(this);
}

void UPlayerCharacterMovementComponent::SetVelocityXY(FVector newVel)
{
	m_Velocity.X = newVel.X;
	m_Velocity.Y = newVel.Z;
}

void UPlayerCharacterMovementComponent::SetVelocityZ(float newVel)
{
	m_Velocity.Z = newVel;
}

void UPlayerCharacterMovementComponent::SetVelocity(FVector newVel)
{
	m_Velocity = newVel;
}

FVector UPlayerCharacterMovementComponent::GetForwardDirection()
{
	AActor* pOwner = GetOwner();
	if (!pOwner) return FVector{};

	return pOwner->GetActorForwardVector();
}

FVector UPlayerCharacterMovementComponent::GetPosMoveDirection() const
{
	return m_PosMoveDirection;
}

FVector UPlayerCharacterMovementComponent::GetFacingDirection() const
{
	return m_FacingDirection;
}

void UPlayerCharacterMovementComponent::SetFacingDirection(const FVector& vec)
{
	m_FacingDirection = vec;
}

void UPlayerCharacterMovementComponent::StopMovementXY()
{
	m_Velocity.X = 0.0f;
	m_Velocity.Y = 0.0f;
}

void UPlayerCharacterMovementComponent::StopMovementZ()
{
	m_Velocity.Z = 0.0f;
}

void UPlayerCharacterMovementComponent::StopMovement()
{
	m_Velocity = FVector{ 0.0f, 0.0f, 0.0f };
}

float UPlayerCharacterMovementComponent::GetMaxSlopeAngle() const
{
	return m_MaxSlopeAngle;
}

int32 UPlayerCharacterMovementComponent::GetMaxNrOfJumps() const
{
	return m_MaxJumpNr;
}

float UPlayerCharacterMovementComponent::GetWallSlideMaxSpeed() const
{
	return m_WallSlideMaxSpeed;
}

float UPlayerCharacterMovementComponent::GetWallSlideAcceleration() const
{
	return m_WallSlideAcceleration;
}

float UPlayerCharacterMovementComponent::GetWallHorJumpSpeed() const
{
	return m_WallHorJumpSpeed;
}

float UPlayerCharacterMovementComponent::GetWallVertJumpSpeed() const
{
	return m_WallVertJumpSpeed;
}

float UPlayerCharacterMovementComponent::GetDeadZoneLeftX() const
{
	return m_DeadZoneLeftX;
}

FVector UPlayerCharacterMovementComponent::GetPositiveYAxis() const
{
	FVector YAxis = FVector::CrossProduct(FVector::UpVector, m_PosMoveDirection);
	return YAxis;
}

float UPlayerCharacterMovementComponent::GetRunHeightTolerance() const
{
	return m_RunHeightTolerance;
}

float UPlayerCharacterMovementComponent::GetLandingVelocityRatio() const
{
	return m_LandingVelocityRatio;
}

void UPlayerCharacterMovementComponent::SetIsOnMovingPlatform(bool b)
{
	m_IsOnMovingPlatform = b;
}

bool UPlayerCharacterMovementComponent::GetIsOnMovingPlatform() const
{
	return m_IsOnMovingPlatform;
}

bool UPlayerCharacterMovementComponent::AddWorldOffset(FVector offset, float minTreshold)
{
	AActor* pOwner = GetOwner();
	if (!pOwner) return false;

	FHitResult hitResult{};
	pOwner->SetActorLocation(pOwner->GetActorLocation() + offset, true, &hitResult);

	if (hitResult.bBlockingHit)
		return false;

	return true;

}

bool UPlayerCharacterMovementComponent::AddWorldOffset(FVector offset, FHitResult& outHit, float minTreshold)
{
	AActor* pOwner = GetOwner();
	if (!pOwner) return false;

	FHitResult hitResult{};
	pOwner->SetActorLocation(pOwner->GetActorLocation() + offset, true, &hitResult);
	outHit = hitResult;

	if (hitResult.bBlockingHit)
		return false;

	return true;
}

FVector UPlayerCharacterMovementComponent::GetVelocity() const
{
	return m_Velocity;
}

FVector UPlayerCharacterMovementComponent::GetVelocityXY() const
{
	return FVector(m_Velocity.X, m_Velocity.Y, 0.0f);
}

float UPlayerCharacterMovementComponent::GetMaxSpeedXY() const
{
	return m_MaxSpeedXY;
}

float UPlayerCharacterMovementComponent::GetSpeedXY() const
{
	FVector temp = m_Velocity;
	temp.Z = 0; //extract the x y information
	return temp.Size();
}

float UPlayerCharacterMovementComponent::GetSpeedZ() const
{
	return m_Velocity.Z; //Extract only the z information
}

float UPlayerCharacterMovementComponent::GetMaxAccelerationXY() const
{
	return m_MaxAccelerationXY;
}

float UPlayerCharacterMovementComponent::GetRunDampingXY() const
{
	return m_RunDampingXY;
}

float UPlayerCharacterMovementComponent::GetAirDampingXY() const
{
	return m_AirDampingXY;
}

float UPlayerCharacterMovementComponent::GetJumpSpeed() const
{
	return m_JumpSpeed;
}

float UPlayerCharacterMovementComponent::GetGravity() const
{
	return m_Gravity;
}



