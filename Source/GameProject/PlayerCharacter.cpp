// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "PlayerCharacterMovementComponent.h"
#include "PlayerCharacterController.h"
#include "PlayerSoul.h"
#include "Engine/World.h"
#include "MyGameInstance.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "DimensionMesh.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MovementComponent = CreateDefaultSubobject<UPlayerCharacterMovementComponent>(FName("PlayerCharacterMovementComponent"));

	PlatformDetector = CreateDefaultSubobject<UBoxComponent>(FName("PlatformDetector"));
	PlatformDetector->SetBoxExtent(FVector(15.0f, 30.0f, 50.0f));
	RootComponent = PlatformDetector;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(FName("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(PlatformDetector);
	SkeletalMeshComponent->SetRelativeScale3D(FVector(4.0f, 4.0f, 4.0f));
	SkeletalMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -PlatformDetector->GetUnscaledBoxExtent().Z));

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(FName("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	SpringArmComponent->TargetArmLength = 750.0f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(FName("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlatformDetector->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnComponentBeginOverlap);
	PlatformDetector->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnComponentEndOverlap);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Update Character Mesh Direction
	USkeletalMeshComponent* pSkelCompScene = FindComponentByClass<USkeletalMeshComponent>();
	if (!pSkelCompScene)
		return;

	FVector forward = MovementComponent->GetFacingDirection();
	FRotator rot = forward.Rotation();
	pSkelCompScene->SetWorldRotation(rot);
	//UE_LOG(LogTemp, Warning, TEXT("Rotation Direction: %s"), *forward.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("Rotation: %s"), *forward.ToString());
	//SetActorRotation(rot);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlayerCharacter::HandleInput(APlayerCharacterController::CharacterInput input, float axis)
{
	switch (input)
	{
	case APlayerCharacterController::CharacterInput::fire:
		break;
	case APlayerCharacterController::CharacterInput::possess:
		HandlePossess();
		break;
	default:
		MovementComponent->HandleInput(input, axis);
		break;
	}


}

void APlayerCharacter::GetPossessed(APlayerSoul* pSoul)
{
	APlayerCharacter* pChar = pSoul->GetSoulOwner();
	if (!pChar) return;

	APlayerCharacterController* pController = Cast<APlayerCharacterController>(pSoul->GetController());
	if (!pController) return;

	UE_LOG(LogTemp, Warning, TEXT("Possess!"));

	FViewTargetTransitionParams transParams{};
	transParams.BlendFunction = EViewTargetBlendFunction::VTBlend_Cubic;
	pController->SetViewTargetWithBlend(this, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic, 0.0f, true);
	pController->Possess(this);

	pSoul->Destroy();
	pController->SetIsInPossessionMode(false);
}

bool APlayerCharacter::GetIsOverlappingDimensionBox() const
{
	return m_IsOverlappingDimensionBox;
}

void APlayerCharacter::GetCrushedByDimensionMesh()
{
	USkeletalMeshComponent* pSkelComp = FindComponentByClass<USkeletalMeshComponent>();
	if(pSkelComp) pSkelComp->SetVisibility(false);
}

void APlayerCharacter::HandlePossess()
{
	UWorld* pWorld = GetWorld();
	if (!pWorld) return;

	UPlayerCharacterMovementComponent* pMoveComp = FindComponentByClass< UPlayerCharacterMovementComponent>();
	if (!pMoveComp) return;

	//Spawn Soul
	FActorSpawnParameters params{};
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	APlayerSoul* pSoul = pWorld->SpawnActor< APlayerSoul>(*UMyGameInstance::PlayerSoulClass,
		GetActorLocation(),
		FRotator::ZeroRotator, params);

	APlayerCharacterController* pCont = Cast<APlayerCharacterController>(GetController());
	if (!pCont) return;

	//Set camera blending parameters before controlling new soul
	if (pSoul)
	{
		//Set Initial Values of pSoul
		pSoul->SetMoveDirection(pMoveComp->GetPosMoveDirection().GetSafeNormal());
		pSoul->SetSoulOwner(this);

		FViewTargetTransitionParams transParams{};
		transParams.BlendFunction = EViewTargetBlendFunction::VTBlend_Cubic;
		pCont->SetViewTargetWithBlend(pSoul, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic, 0.0f, true);

		pCont->Possess(pSoul);
		pCont->SetIsInPossessionMode(true);
	}

	pCont->OnSpawnSoul.Broadcast(pSoul);
}

void APlayerCharacter::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ADimensionMesh::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("OnComponentBeginOverlap!"));
		m_IsOverlappingDimensionBox = true;
	}
}

void APlayerCharacter::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(ADimensionMesh::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("OnComponentEndOverlap!"));
		m_IsOverlappingDimensionBox = false;
	}
}
