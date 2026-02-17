// Fill out your copyright notice in the Description page of Project Settings.


#include "TankClasses/TankBullet.h"

// Sets default values
ATankBullet::ATankBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	BulletMesh->SetupAttachment(GetRootComponent());
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxComponent->SetupAttachment(BulletMesh);

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

