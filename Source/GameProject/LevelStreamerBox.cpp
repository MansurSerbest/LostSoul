// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelStreamerBox.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacterController.h"
#include "FLoadDimensionLevelStream.h"

// Sets default values
ALevelStreamerBox::ALevelStreamerBox()
	:m_pPlayerController{ nullptr }, m_DimensionLevelName{}
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LoadBoxComponent = CreateDefaultSubobject<UBoxComponent>(FName("LoadBoxComponent"));
	RootComponent = LoadBoxComponent;
	RootComponent->SetWorldLocation(FVector::ZeroVector);

	UnloadBoxComponent = CreateDefaultSubobject<UBoxComponent>(FName("UnloadBoxComponent"));
	UnloadBoxComponent->SetupAttachment(RootComponent);
	UnloadBoxComponent->SetRelativeLocation(FVector::ZeroVector);

	LoadBoxComponent->SetActive(true);
	UnloadBoxComponent->SetActive(false);
}

// Called when the game starts or when spawned
void ALevelStreamerBox::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("ALevelStreamerBox::BeginPlay()"));


	Super::BeginPlay();
	if (LoadBoxComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Add Dynamic LoadLevel"));
		LoadBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ALevelStreamerBox::LoadLevelByBeginOverlap);
	}

	if (UnloadBoxComponent)
	{
		UnloadBoxComponent->Deactivate();
		UE_LOG(LogTemp, Warning, TEXT("Add Dynamic UnloadLevel"));
		UnloadBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ALevelStreamerBox::UnloadLevelByBeginOverlap);
	}

	m_pPlayerController = Cast<APlayerCharacterController>(GetWorld()->GetFirstPlayerController());

	//FString dimensionSuffix = TEXT("_Dimension");
	FString dimensionLevelName{m_LevelName.ToString() + TEXT("_Dimension") };
	UE_LOG(LogTemp, Warning, TEXT("dimension name: %s"), *dimensionLevelName);
	m_DimensionLevelName = FName(*dimensionLevelName);
}

// Called every frame
void ALevelStreamerBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelStreamerBox::LoadLevelByBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (LoadBoxComponent->IsActive())
	{
		UE_LOG(LogTemp, Warning, TEXT("ALevelStreamerBox::LoadLevel"));
		if (m_pPlayerController->GetIsDimension())
		{
			UGameplayStatics::LoadStreamLevel(GetWorld(), m_LevelName, true, true, FLatentActionInfo{});
			UGameplayStatics::LoadStreamLevel(GetWorld(), m_DimensionLevelName, true, true, FLatentActionInfo{});
		}
		else
		{
			UGameplayStatics::LoadStreamLevel(GetWorld(), m_LevelName, true, true, FLatentActionInfo{});
		}
		UnloadBoxComponent->Activate();
		LoadBoxComponent->Deactivate();
	}

}

void ALevelStreamerBox::UnloadLevelByBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (UnloadBoxComponent->IsActive())
	{
		if (m_pPlayerController->GetIsDimension())
		{
			UGameplayStatics::UnloadStreamLevel(GetWorld(), m_LevelName, FLatentActionInfo{}, false);

			UGameplayStatics::UnloadStreamLevel(GetWorld(), m_DimensionLevelName, FLatentActionInfo{}, false);
			UE_LOG(LogTemp, Warning, TEXT("UNLOAD DIMENSIONLEVEL: %s"), *m_DimensionLevelName.ToString());
		}
		else
		{
			UGameplayStatics::UnloadStreamLevel(GetWorld(), m_LevelName, FLatentActionInfo{}, true);
		}
		UnloadBoxComponent->Deactivate();
		LoadBoxComponent->Activate();
	}

}

void ALevelStreamerBox::UnloadlevelByEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

