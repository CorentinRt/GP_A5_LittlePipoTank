// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "Server/Game/GameModeTankServer.h"
#include "NiagaraFunctionLibrary.h"
#include "Shared/Game/GamePhaseListener.h"
#include "TankPawn.generated.h"

class UBoxComponent;
class ATankBullet;
class USceneComponent;

UCLASS()
class GP_A5_LITTLEPIPOTANK_API ATankPawn : public APawn, public IGamePhaseListener
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	TObjectPtr<UBoxComponent> TankBoxCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Tank")
	UStaticMeshComponent* TankHeadMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Tank")
	UStaticMeshComponent* TankBodyMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Tank")
	USceneComponent* TankBodyMeshHolder;
	UPROPERTY(EditAnywhere, Category= "Tank")
	USceneComponent* TankShootingPoint;
	UPROPERTY(EditAnywhere, Category= "Tank")
	UStaticMesh* BulletMesh;
	UPROPERTY(EditAnywhere, Category= "Tank")
	uint8 sizeOfBullet = 3;
	UPROPERTY(EditAnywhere, Category="Tank")
	class UNiagaraSystem* TankParticleExplosion;
	
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
	
	UPROPERTY(VisibleAnywhere)
	FPlayerTankInputs TankInputs;
private:
	UPROPERTY(VisibleAnywhere, Category="Tank")
	AGameModeTankServer* GameMode;

public:
	// Sets default values for this character's properties
	ATankPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Destroyed() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetPlayerTankInputs(const FPlayerTankInputs& InTankInputs);
	
protected:
	void MoveTank(float MoveInput, float DeltaTime, bool UseSweep = true);

	void RotateTank(float RotateInput, float DeltaTime);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bBlockAllInputs = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ATankBullet> TankBulletClassBlueprint;

	UFUNCTION()
	bool CanShoot() const;

	void UpdateShootCooldown(float DeltaTime);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CooldownBetweenEachShoot = 1.f;
	
	float DelayBeforeShootingAvailable = 0.f;
	
public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpawnBullet, ATankBullet*, SpawnedTankBullet);
	FOnSpawnBullet OnSpawnBullet;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTankDestroyed, ATankPawn*, InTank);
	FOnTankDestroyed OnTankDestroyed;
	
	float GetHeadAimRotation() const;
	float GetTankRotation() const;
	FVector2D GetTankLocation() const;

	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RotateSpeed = 100.f;
	
	virtual void UpdatePhysics(float DeltaTime, bool UseSweep = true);

	const FPlayerTankInputs& GetTankInputs() const;
	
	void TankGetShoot();
	
	UFUNCTION(BlueprintImplementableEvent)
	void ReceiveGetShoot();
	
	UFUNCTION(BlueprintImplementableEvent)
	void ReceiveShooting(ATankBullet* outBullet);

	virtual void ReactOnGamePhaseChanged_Implementation(ETankGamePhase InGamePhase) override;
};
