// Fill out your copyright notice in the Description page of Project Settings.


#include "TankClasses/TankBullet.h"

#include "TankPawn.h"
#include "Chaos/Utilities.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ATankBullet::ATankBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	RootComponent = BulletMesh;
	BulletMesh->SetSimulatePhysics(false);
	BulletMesh->SetUsingAbsoluteRotation(false);
	//BulletMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
	ProjectileMovement->InitialSpeed = 350.f;
	ProjectileMovement->MaxSpeed = 350.f;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->SetUpdatedComponent(RootComponent);
	ProjectileMovement->bRotationFollowsVelocity = true;
	
	ProjectileMovement->OnProjectileBounce.AddDynamic(this, &ATankBullet::OnBounce);
	
	numberOfBouncesLeft = numberOfBounces;
}

// Called when the game starts or when spawned
void ATankBullet::BeginPlay()
{
	Super::BeginPlay();
	ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileMovement->InitialSpeed;
	if (GetInstigator())
	{
		BulletMesh->IgnoreActorWhenMoving(GetInstigator(), true); //Ignore le big tank 
	}
}

void ATankBullet::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	numberOfBouncesLeft--;
	AActor* HitActor = ImpactResult.GetActor();
	ATankPawn* HitPawn = Cast<ATankPawn>(HitActor);
	if (HitPawn)
	{
		HitPawn->TankGetShoot();
	}
	ATankBullet* HitBulletPawn = Cast<ATankBullet>(HitActor);
	if (HitBulletPawn)
	{
		HitBulletPawn->Destroy();
		this->Destroy();
	}
	if (this && numberOfBouncesLeft == 0)
	{
		this->Destroy();
	}
}

// Called every frame
void ATankBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
