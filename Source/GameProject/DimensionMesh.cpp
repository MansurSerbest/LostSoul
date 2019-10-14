// Fill out your copyright notice in the Description page of Project Settings.


#include "DimensionMesh.h"
#include "Components/StaticMeshComponent.h"
#include "PlayerCharacterController.h"
#include "PlayerCharacter.h"

// Sets default values
ADimensionMesh::ADimensionMesh()
	:m_PlayerCharacter{nullptr}
{
 	//Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshRootComponent = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	RootComponent = MeshRootComponent;

	NormalDimensionMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("NormalDimensionMeshComponent"));
	NormalDimensionMeshComponent->SetupAttachment(RootComponent);
	NormalDimensionMeshComponent->SetRelativeLocation(FVector::ZeroVector);

	SoulDimensionMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("SoulDimensionMeshComponent"));
	SoulDimensionMeshComponent->SetupAttachment(RootComponent);
	SoulDimensionMeshComponent->SetRelativeLocation(FVector::ZeroVector);
	
}

// Called when the game starts or when spawned
void ADimensionMesh::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerCharacterController *pCont = Cast<APlayerCharacterController>(GetWorld()->GetFirstPlayerController());
	if (pCont)
	{
		pCont->OnSwitchToNormalDimension.AddDynamic(this, &ADimensionMesh::SwitchToNormalDimension);
		pCont->OnSwitchToSoulDimension.AddDynamic(this, &ADimensionMesh::SwitchToSoulDimension);		
	}

	SoulDimensionMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called every frame
void ADimensionMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void ADimensionMesh::SetCharacter(APlayerCharacter* pCharacter)
{
	m_PlayerCharacter = pCharacter;
}

void ADimensionMesh::SwitchToSoulDimension_Implementation()
{
	if (m_ChangeVisibilityOnSwitch)
	{
		NormalDimensionMeshComponent->SetVisibility(false);
		SoulDimensionMeshComponent->SetVisibility(true);
	}

	//NormalDimensionMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NormalDimensionMeshComponent->SetCollisionProfileName(FName("OverlapAll"));
	SoulDimensionMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ADimensionMesh::SwitchToNormalDimension_Implementation()
{
	if (m_ChangeVisibilityOnSwitch)
	{
		NormalDimensionMeshComponent->SetVisibility(true);
		SoulDimensionMeshComponent->SetVisibility(false);
	}

	//NormalDimensionMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	NormalDimensionMeshComponent->SetCollisionProfileName(FName("BlockAllDynamic"));
	SoulDimensionMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


