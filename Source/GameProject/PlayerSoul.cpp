// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSoul.h"
#include "Engine/World.h"
#include "Components/SphereComponent.h"
#include "Possessable.h"
#include "GameFramework/Actor.h"
#include "PlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
APlayerSoul::APlayerSoul()
	:m_MaxAcceleration{ 500.0f }, m_MaxSpeed{ 100.0f }, m_Damping{200.0f}, m_Velocity{}, m_MoveDirection{}, m_pOwner{ nullptr }
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//RootComponent = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("StaticMeshComponent"));
	RootComponent = StaticMeshComponent;

	RadarSphereComponent = CreateDefaultSubobject<USphereComponent>(FName("RadarComponent"));
	if (!RadarSphereComponent)
		UE_LOG(LogTemp, Error, TEXT("APlayerSoul::APlayerSoul: RadarSphereComponenet could not be initialized!"));

	RadarSphereComponent->SetupAttachment(RootComponent);
	RadarSphereComponent->SetSphereRadius(100.0f);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(FName("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(FName("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

}

// Called when the game starts or when spawned
void APlayerSoul::BeginPlay()
{
	Super::BeginPlay();

	RadarSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &APlayerSoul::OnComponentBeginOverlap);
	m_SpawnLocation = GetActorLocation();
}

// Called every frame
void APlayerSoul::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if ((!m_IsHorAxisInput) && (!m_IsVertAxisInput))
		DampActor(DeltaTime);

	m_IsHorAxisInput = false;
	m_IsVertAxisInput = false;
}

// Called to bind functionality to input
void APlayerSoul::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlayerSoul::SetMoveDirection(FVector dir)
{
	m_MoveDirection = dir.GetSafeNormal();
}

void APlayerSoul::GetPossessed(APlayerSoul* pSoul)
{

}

void APlayerSoul::HandleInput(APlayerCharacterController::CharacterInput input, float axis)
{
	switch (input)
	{
	case APlayerCharacterController::CharacterInput::moveLeftRight:
		if (axis > 0.0f) m_IsHorAxisInput = true;
		HandleLeftRight(axis);
		break;
	case APlayerCharacterController::CharacterInput::moveDownUp:
		if (axis > 0.0f) m_IsVertAxisInput = true;
		HandleDownUp(axis);
		break;
	case APlayerCharacterController::CharacterInput::possess:
		HandlePosses();
		break;
	default:
		break;
	}
}

void APlayerSoul::SetSoulOwner(APlayerCharacter* pPlayer)
{
	m_pOwner = pPlayer;
	if (!m_pOwner)
		UE_LOG(LogTemp, Warning, TEXT("APlayerSoul::SetOwner: m_pOwner is not valid!"));
}

APlayerCharacter* APlayerSoul::GetSoulOwner() const
{
	return m_pOwner;
}

void APlayerSoul::HandleLeftRight(float axis)
{
	//Get delta time
	UWorld* pWorld = GetWorld();
	if (!pWorld) return;
	float dt = pWorld->GetDeltaSeconds();

	//Get standard forward direction and multiply it with axis and acc..
	m_Velocity += m_MoveDirection * m_MaxAcceleration * axis * dt;
	m_Velocity = m_Velocity.GetClampedToMaxSize(m_MaxSpeed);

	FHitResult hitResult{};
	AddActorWorldOffset(FVector(m_Velocity.X * dt, m_Velocity.Y * dt, 0.0f), true, &hitResult);
	if (hitResult.IsValidBlockingHit())
	{
		m_Velocity.X = 0.0f;
		m_Velocity.Y = 0.0f;
	}
	else if ((GetActorLocation() - m_SpawnLocation).Size() > m_MoveRadius)
	{
		AddActorWorldOffset(FVector(-m_Velocity.X * dt, -m_Velocity.Y * dt, 0.0f), true);
		m_Velocity = FVector::ZeroVector;
	}

}

void APlayerSoul::DampActor(float dt)
{
	UE_LOG(LogTemp, Warning, TEXT("[%f]Damping Soul!!"), GetWorld()->GetTimeSeconds());

	//If speed is under a certain treshold, stop the movement completely
	float minSpeed{ 0.1f };
	if (m_Velocity.Size() < minSpeed)
	{
		m_Velocity = FVector::ZeroVector;
	}


	//Get velocity and take the opposite direction. Add the world offset
	FVector offset = m_Velocity.GetSafeNormal();
	offset *= (-1.0f) * m_Damping * dt;
	offset = offset.GetClampedToSize(0.0f, m_Velocity.Size() * 0.9); // This way, the offset will never go to the opposite side 

	FVector diff = m_Velocity + offset;

	AddActorWorldOffset(offset * dt);
	m_Velocity = diff;
}

void APlayerSoul::HandleDownUp(float axis)
{
	//Get delta time
	UWorld* pWorld = GetWorld();
	if (!pWorld) return;
	float dt = pWorld->GetDeltaSeconds();

	//Get standard forward direction and multiply it with axis and acc..
	m_Velocity.Z += m_MaxAcceleration * axis * dt;
	m_Velocity = m_Velocity.GetClampedToMaxSize(m_MaxSpeed);

	FHitResult hitResult{};
	AddActorWorldOffset(FVector(0.0f, 0.0f, m_Velocity.Z * dt), true, &hitResult);
	if (hitResult.IsValidBlockingHit())
	{
		m_Velocity.Z = 0.0f;
	}
	else if ((GetActorLocation() - m_SpawnLocation).Size() > m_MoveRadius)
	{
		AddActorWorldOffset(FVector(0.0f, 0.0f, -m_Velocity.Z * dt), true);
		m_Velocity = FVector::ZeroVector;
	}
}

void APlayerSoul::HandlePosses()
{
	TArray<AActor*> overlappingPawns{};
	if (IsPossessableDetected(overlappingPawns))
	{
		AActor* pOwnerActor = Cast<AActor>(m_pOwner);
		if (!pOwnerActor) return;

		//if (m_pOwner == OtherActor)	return;

		IPossessable* pPossessable = Cast<IPossessable>(overlappingPawns[0]);
		if (pPossessable)
		{
			pPossessable->GetPossessed(this);
		}
	}
}

bool APlayerSoul::IsPossessableDetected(TArray<AActor*>& overlappingActors)
{
	TArray<AActor*> overlappingPawns{};
	RadarSphereComponent->GetOverlappingActors(overlappingPawns, TSubclassOf<APawn>());

	if (overlappingPawns.Num() > 0)
	{
		overlappingActors = overlappingPawns;
		return true;
	}

	return false;
}

void APlayerSoul::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

