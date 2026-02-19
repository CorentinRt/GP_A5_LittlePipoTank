// Fill out your copyright notice in the Description page of Project Settings.


#include "Client/Game/ClientTankPawn.h"


// Sets default values
AClientTankPawn::AClientTankPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AClientTankPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AClientTankPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AClientTankPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AClientTankPawn::SetLocation(const FVector2D& Location)
{
	FVector NewLocation(Location.X, Location.Y, 0.0f);
	SetActorLocation(NewLocation, true);
}

void AClientTankPawn::SetRotation(const FRotator& Rotation)
{
	SetActorRotation(Rotation);
}

void AClientTankPawn::SetAimRotation(float AimRotation)
{
	
}

