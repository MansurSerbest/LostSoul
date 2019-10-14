// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerCamera.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class GAMEPROJECT_API APlayerCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerCamera();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleDefaultsOnly, meta = (DisplayName = "CameraComponent"))
	UCameraComponent* pCameraComponent;

	UPROPERTY(VisibleDefaultsOnly, meta = (DisplayName = "SpringArmComponent"))
		USpringArmComponent* pSpringArmComponent;
};
