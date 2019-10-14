// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DimensionMesh.generated.h"

class UStaticMeshComponent;
class APlayerCharacter;

UCLASS()
class GAMEPROJECT_API ADimensionMesh : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADimensionMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetCharacter(APlayerCharacter* pCharacter);

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SwitchToSoulDimension();
	void SwitchToSoulDimension_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SwitchToNormalDimension();
	void SwitchToNormalDimension_Implementation();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* MeshRootComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent *NormalDimensionMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* SoulDimensionMeshComponent;

	UPROPERTY(EditAnywhere, Category = Switch, meta = (DisplayName = "ChangeVisibilityOnSwitch"))
	bool m_ChangeVisibilityOnSwitch{ true };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Switch, meta = (DisplayName = "IsFloating"))
	bool m_IsFloating{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character, meta = (DisplayName = "PlayerCharacter"))
	APlayerCharacter *m_PlayerCharacter{ false };
};
