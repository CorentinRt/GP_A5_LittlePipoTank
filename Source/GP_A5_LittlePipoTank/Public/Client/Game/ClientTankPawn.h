// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TankClasses/TankPawn.h"
#include "ClientTankPawn.generated.h"

UCLASS()
class GP_A5_LITTLEPIPOTANK_API AClientTankPawn : public ATankPawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AClientTankPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetLocation(const FVector2D& Location, bool Sweep);
	void SetRotation(const FRotator& Rotation);
	void SetAimRotation(const FRotator& AimRotation);

	UFUNCTION(BlueprintImplementableEvent)
	void SetMeshMaterial(const UMaterialInterface* Material);
};
