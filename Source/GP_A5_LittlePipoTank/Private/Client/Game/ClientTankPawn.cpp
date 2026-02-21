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

void AClientTankPawn::UpdatePhysics(float DeltaTime)
{
	if (IsHidden())
		return;
	
	// UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Tick physi tank");
	//Move
	MoveTank(bBlockAllInputs ? 0.f : TankInputs.MoveInput.Y, DeltaTime);

	// Rotation Tank
	RotateTank(bBlockAllInputs ? 0.f : TankInputs.MoveInput.X, DeltaTime);
	
	//Aim Rotation
	FRotator CurrentWorldRot = TankHeadMesh->GetComponentRotation();
	FVector AimFVector {TankInputs.AimInput.X, TankInputs.AimInput.Y, 0.0f};
	TargetWorldRotation =  FRotationMatrix::MakeFromX(AimFVector).Rotator();
	FRotator SmoothedRot = FMath::RInterpTo(
		CurrentWorldRot,
		TargetWorldRotation,
		DeltaTime,
		HeadRotationSpeed
	);
	TankHeadMesh->SetWorldRotation(SmoothedRot);
}

void AClientTankPawn::SetLocation(const FVector2D& Location, bool Sweep)
{
	FVector NewLocation(Location.X, Location.Y, 0.0f);
	SetActorLocation(NewLocation, Sweep);
}

void AClientTankPawn::SetRotation(const FRotator& Rotation)
{
	SetActorRotation(Rotation);
}

void AClientTankPawn::SetAimRotation(const FRotator& AimRotation)
{
	TankHeadMesh->SetWorldRotation(AimRotation);
}

