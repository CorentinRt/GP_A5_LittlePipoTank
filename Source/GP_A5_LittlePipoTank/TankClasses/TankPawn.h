// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Server/Game/GameModeTankServer.h"
#include "Shared/Game/PhysicsTickableShared.h"
#include "TankPawn.generated.h"

UCLASS()
class GP_A5_LITTLEPIPOTANK_API ATankPawn : public APawn, public IPhysicsTickableShared
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category= "Tank")
	UStaticMeshComponent* TankHeadMesh;
	UPROPERTY(EditAnywhere, Category= "Tank")
	UStaticMeshComponent* TankBodyMesh;
	UPROPERTY(EditAnywhere, Category= "Tank")
	USceneComponent* TankShootingPoint;
	UPROPERTY(EditAnywhere, Category= "Tank")
	UStaticMesh* BulletMesh;
	UPROPERTY(EditAnywhere, Category= "Tank")
	uint8 sizeOfBullet = 3;
	
protected:
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* AimAction;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* ShootAction;

	UPROPERTY(EditAnywhere, Category = "TankHead")
	float HeadRotationSpeed = 20.f;
	
	FRotator TargetWorldRotation;

private:
	UPROPERTY(VisibleAnywhere, Category="Tank")
	AGameModeTankServer* GameMode;

	UPROPERTY(VisibleAnywhere)
	FPlayerTankInputs TankInputs;

public:
	// Sets default values for this character's properties
	ATankPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	void Move(const FInputActionValue& Value);
	void Aim(const FInputActionValue& Value);
	void Shoot(const FInputActionValue& Value);

	void MoveTank(float MoveInput, float DeltaTime);

	void RotateTank(float RotateInput, float DeltaTime);
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RotateSpeed = 100.f;
	
	virtual void RegisterTickable() override;
	virtual void UnregisterTickable() override;
	virtual void OnTickPhysics_Blueprint_Implementation(float DeltaTime) override;

	const FPlayerTankInputs& GetTankInputs() const;
	
	void TankGetShoot();
};
