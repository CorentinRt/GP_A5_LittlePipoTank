// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TankBullet.generated.h"

UCLASS()
class GP_A5_LITTLEPIPOTANK_API ATankBullet : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category= "Bullet")
	UStaticMeshComponent* BulletMesh;
	
	UPROPERTY(EditAnywhere, Category= "Bullet")
	float BulletSpeed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Bullet")
	UProjectileMovementComponent* ProjectileMovement;
	
	// Sets default values for this actor's properties
	ATankBullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, Category= "Bullet")
	uint8 numberOfBounces = 3;
private:
	int numberOfBouncesLeft;

};
