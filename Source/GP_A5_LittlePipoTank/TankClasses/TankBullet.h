// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shared/Game/GamePhaseListener.h"
#include "TankBullet.generated.h"

class USphereComponent;
class AGameModeTankServer;

UCLASS(BlueprintType)
class GP_A5_LITTLEPIPOTANK_API ATankBullet : public AActor, public IGamePhaseListener
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category= "Bullet")
	UStaticMeshComponent* BulletMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> SphereCollision;
	
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
	
	void UpdatePhysics(float DeltaTime);
	bool CheckBulletDestroyed();
	
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

	UPROPERTY()
	bool bMarkedForDestroy = false;

};
