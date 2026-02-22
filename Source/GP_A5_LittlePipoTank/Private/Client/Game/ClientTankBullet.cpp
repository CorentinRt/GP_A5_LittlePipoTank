// Fill out your copyright notice in the Description page of Project Settings.


#include "Client/Game/ClientTankBullet.h"


// Sets default values
AClientTankBullet::AClientTankBullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	RootComponent = BulletMesh;
	BulletMesh->SetSimulatePhysics(false);
	// BulletMesh->SetUsingAbsoluteRotation(false);
	BulletMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AClientTankBullet::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AClientTankBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AClientTankBullet::SetLocation(const FVector& NewLocation, bool Sweep)
{
	SetActorLocation(FVector(NewLocation.X, NewLocation.Y, NewLocation.Z), Sweep);
}

void AClientTankBullet::SetRotation(const FRotator& NewRotation)
{
	SetActorRotation(NewRotation);
}

