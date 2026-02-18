// Fill out your copyright notice in the Description page of Project Settings.


#include "TankClasses/TankBullet.h"

#include "Chaos/Utilities.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ATankBullet::ATankBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BulletSpeed = 5.0f;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	RootComponent = BulletMesh;
	BulletMesh->SetSimulatePhysics(false);
	BulletMesh->SetUsingAbsoluteRotation(false);
	//BulletMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
	ProjectileMovement->InitialSpeed = 50.f;
	ProjectileMovement->MaxSpeed = 50.f;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->SetUpdatedComponent(RootComponent);

}

// Called when the game starts or when spawned
void ATankBullet::BeginPlay()
{
	Super::BeginPlay();
	ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileMovement->InitialSpeed;
}

// Called every frame
void ATankBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

FVector ATankBullet::CalculateBulletImpact(FVector BulletDirection, FVector ImpactPoint)
{
	FVector returnValue = BulletDirection - ImpactPoint * 2 * (BulletDirection | ImpactPoint);
	return returnValue;
}

