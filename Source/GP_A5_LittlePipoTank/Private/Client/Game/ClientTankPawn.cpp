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

	LocationVisualError *= 0.9f;
	RotationVisualError *= 0.9f;

	TankBodyMeshHolder->SetRelativeLocation(LocationVisualError);
	TankBodyMeshHolder->SetRelativeRotation(RotationVisualError);
}

// Called to bind functionality to input
void AClientTankPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AClientTankPawn::UpdatePhysics(float DeltaTime, bool UseSweep)
{
	if (IsHidden())
		return;
	
	// UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Tick physi tank");
	//Move

	FVector PreviousLocation = GetActorLocation();
	
	MoveTank(bBlockAllInputs ? 0.f : TankInputs.MoveInput.Y, DeltaTime, UseSweep);

	if ((PreviousLocation - GetActorLocation()).Length() > MinDistanceToConsiderIsMoving)
	{
		ReceiveOnTankMoveBlueprint();
	}
	
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
	FVector NewLocation(Location.X, Location.Y, GetActorLocation().Z);

	if ((NewLocation - GetActorLocation()).Length() > MinDistanceToConsiderIsMoving)
	{
		ReceiveOnTankMoveBlueprint();
	}
	
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

void AClientTankPawn::AddVisualError(const FVector& LocationError, const FRotator& RotationError)
{
	LocationVisualError += LocationError;
	RotationVisualError += RotationError;
}

FVector AClientTankPawn::GetVisualLocation() const
{
	return GetActorLocation() + LocationVisualError;
}

FRotator AClientTankPawn::GetVisualRotation() const
{
	return GetActorRotation() + RotationVisualError;
}


