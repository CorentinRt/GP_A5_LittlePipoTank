// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ClientTankBullet.generated.h"

UCLASS()
class GP_A5_LITTLEPIPOTANK_API AClientTankBullet : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AClientTankBullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetLocation(const FVector2D& NewLocation, bool Sweep);
	void SetRotation(const FRotator& NewRotation);
	
private:
	UPROPERTY(EditAnywhere, Category= "Bullet")
	UStaticMeshComponent* BulletMesh;
};
