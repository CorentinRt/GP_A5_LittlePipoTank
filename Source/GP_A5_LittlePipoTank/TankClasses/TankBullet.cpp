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
	BulletMesh->SetupAttachment(GetRootComponent());
	BulletMesh->SetSimulatePhysics(false);
	BulletMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootComponent = BulletMesh;
	

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxComponent->SetupAttachment(BulletMesh);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
	ProjectileMovement->InitialSpeed = 2000.f;
	ProjectileMovement->MaxSpeed = 2000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->SetUpdatedComponent(RootComponent);

	ProjectileMovement->SetVelocityInLocalSpace(FVector::ForwardVector * ProjectileMovement->InitialSpeed);
}

// Called when the game starts or when spawned
void ATankBullet::BeginPlay()
{
	Super::BeginPlay();
	
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

