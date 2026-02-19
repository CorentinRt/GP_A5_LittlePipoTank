// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Shared/Game/GamePhaseListener.h"
#include "Shared/Game/PhysicsTickableShared.h"
#include "TankBullet.generated.h"

class AGameModeTankServer;

UCLASS()
class GP_A5_LITTLEPIPOTANK_API ATankBullet : public AActor, public IPhysicsTickableShared, public IGamePhaseListener
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category= "Bullet")
	UStaticMeshComponent* BulletMesh;
	
	UPROPERTY(EditAnywhere, Category= "Bullet")
	float BulletSpeed = 200.f;
	
	// Sets default values for this actor's properties
	ATankBullet();

	int BulletIndex = -1;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Destroyed() override;
	
	void HandleBounce(const FHitResult& Hit);
	
	UFUNCTION()
	void OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	void SetBulletVelocity(const FVector& InVelocity);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void RegisterTickable() override;
	virtual void UnregisterTickable() override;
	virtual void OnTickPhysics_Blueprint_Implementation(float DeltaTime) override;
	
	virtual void RegisterListener() override;
	virtual void UnregisterListener() override;
	virtual void ReactOnGamePhaseChanged_Implementation(ETankGamePhase InGamePhase) override;

	UPROPERTY(EditAnywhere, Category= "Bullet")
	uint8 numberOfBounces = 3;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBulletDestroyed, ATankBullet*, InBulletDestroyed);

	FOnBulletDestroyed OnBulletDestroyed;
	
private:
	int numberOfBouncesLeft;

	FVector Velocity = FVector::ZeroVector;

	UPROPERTY()
	TObjectPtr<AGameModeTankServer> GameMode;

};
