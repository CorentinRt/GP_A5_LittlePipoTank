// Fill out your copyright notice in the Description page of Project Settings.


#include "Server/Game/PlayerTankSpawnPoint.h"


// Sets default values
APlayerTankSpawnPoint::APlayerTankSpawnPoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void APlayerTankSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerTankSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

