// Fill out your copyright notice in the Description page of Project Settings.


#include "TankClasses/TankBullet.h"

#include "TankPawn.h"
#include "Kismet/GameplayStatics.h"

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
	
	numberOfBouncesLeft = numberOfBounces;
}

// Called when the game starts or when spawned
void ATankBullet::BeginPlay()
{
	Super::BeginPlay();

	SetBulletVelocity(GetActorForwardVector() * BulletSpeed);
	
	if (GetInstigator())
	{
		BulletMesh->IgnoreActorWhenMoving(GetInstigator(), true); //Ignore le big tank 
	}
}

void ATankBullet::Destroyed()
{
	Super::Destroyed();

	OnBulletDestroyed.Broadcast(this);
}

void ATankBullet::HandleBounce(const FHitResult& Hit)
{
	const FVector Normal = Hit.ImpactNormal.GetSafeNormal();

	SetBulletVelocity(Velocity.MirrorByVector(Normal));

	SetActorLocation(Hit.ImpactPoint + Normal * 2.f);
}

void ATankBullet::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	numberOfBouncesLeft--;
	
	AActor* HitActor = ImpactResult.GetActor();
	
	if (ATankPawn* HitPawn = Cast<ATankPawn>(HitActor))
	{
		HitPawn->TankGetShoot();
		bMarkedForDestroy = true;
		return;
	}

	if (ATankBullet* HitBulletPawn = Cast<ATankBullet>(HitActor))
	{
		HitBulletPawn->Destroy();
		bMarkedForDestroy = true;
		return;
	}
	
	if (numberOfBouncesLeft == 0)
	{
		bMarkedForDestroy = true;
	}
}

void ATankBullet::SetBulletVelocity(const FVector& InVelocity)
{
	Velocity = InVelocity;

	SetActorRotation(Velocity.Rotation());
}

// Called every frame
void ATankBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void ATankBullet::UpdatePhysics(float DeltaTime)
{
	if (bMarkedForDestroy)
		return;
	
	FVector Start = GetActorLocation();

	FVector End = Start + Velocity * DeltaTime;

	FHitResult Hit;

	SetActorLocation(End, true, &Hit);

	if (Hit.bBlockingHit)
	{
		HandleBounce(Hit);
		OnBounce(Hit, Velocity);
	}
}

bool ATankBullet::CheckBulletDestroyed()
{
	if (bMarkedForDestroy && !IsPendingKillPending())
	{
		Destroy();
		return true;
	}

	return false;
}

void ATankBullet::ReactOnGamePhaseChanged_Implementation(ETankGamePhase InGamePhase)
{
	IGamePhaseListener::ReactOnGamePhaseChanged_Implementation(InGamePhase);
	
}
