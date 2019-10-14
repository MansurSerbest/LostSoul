// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelStreamerBox.generated.h"

class UBoxComponent;
class APlayerCharacterController;

UCLASS()
class GAMEPROJECT_API ALevelStreamerBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelStreamerBox();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void LoadLevelByBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void UnloadLevelByBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void UnloadlevelByEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	
public:	
	UPROPERTY(EditInstanceOnly, meta = (DisplayName = "LevelName"))
		FName m_LevelName;

	UPROPERTY(VisibleInstanceOnly)
		UBoxComponent* LoadBoxComponent;

	UPROPERTY(VisibleInstanceOnly)
		UBoxComponent* UnloadBoxComponent;

private:
	APlayerCharacterController* m_pPlayerController;
	FName m_DimensionLevelName;

};
