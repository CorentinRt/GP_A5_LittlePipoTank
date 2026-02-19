#include "TankClasses/TankPawn.h"

#include "AIHelpers.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GP_A5_LittlePipoTank.h"
#include "TankBullet.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Server/Game/GameModeTankServer.h"

// Sets default values
ATankPawn::ATankPawn()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Body
	TankBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyOfTank"));
	TankBodyMesh->SetupAttachment(GetRootComponent());

	//Head
	TankHeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadOfTank"));
	TankHeadMesh->SetupAttachment(TankBodyMesh);
	TankHeadMesh->SetUsingAbsoluteRotation(true);

	//Shooting Point
	TankShootingPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ShootingPoint"));
	TankShootingPoint->SetupAttachment(TankHeadMesh);

}

// Called when the game starts or when spawned
void ATankPawn::BeginPlay()
{
	Super::BeginPlay();

	RegisterTickable();
}

void ATankPawn::Destroyed()
{
	Super::Destroyed();

	UnregisterTickable();
}

// Called every frame
void ATankPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATankPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/*
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller)) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}

	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATankPawn::Move);
		Input->BindAction(MoveAction, ETriggerEvent::Completed, this, &ATankPawn::Move);
		Input->BindAction(AimAction, ETriggerEvent::Triggered, this, &ATankPawn::Aim);
		Input->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ATankPawn::Shoot);
	}
	*/
}

void ATankPawn::SetPlayerTankInputs(const FPlayerTankInputs& InTankInputs)
{
	TankInputs = InTankInputs;
}

void ATankPawn::MoveTank(float MoveInput, float DeltaTime)
{
	FVector Dir = GetActorForwardVector();

	FVector LastActorLocation = GetActorLocation();

	SetActorLocation(LastActorLocation + Speed * Dir * MoveInput * DeltaTime, true);
}

void ATankPawn::RotateTank(float RotateInput, float DeltaTime)
{
	FRotator RotationDelta = {0.f, RotateInput, 0.f};
	AddActorLocalRotation(RotationDelta * DeltaTime * RotateSpeed);
}

void ATankPawn::RegisterTickable()
{
	GameMode = Cast<AGameModeTankServer>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		GameMode->RegisterPhysicsTickable(this);
	}
}

void ATankPawn::UnregisterTickable()
{
	if (GameMode)
	{
		GameMode->UnregisterPhysicsTickable(this);
	}
}

void ATankPawn::OnTickPhysics_Blueprint_Implementation(float DeltaTime)
{
	IPhysicsTickableShared::OnTickPhysics_Blueprint_Implementation(DeltaTime);
	
	//Move
	MoveTank(TankInputs.MoveInput.Y, DeltaTime);

	// Rotation Tank
	RotateTank(TankInputs.MoveInput.X, DeltaTime);
	
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

	//Fire Input
	if(TankInputs.FireInput == true)
	{
		FVector Location(TankShootingPoint->GetComponentLocation());
		FRotator Rotation(TankShootingPoint->GetComponentRotation());
		Rotation.Yaw += 90;
		FActorSpawnParameters SpawnParameters;
		AActor* Bullet = GetWorld()->SpawnActor<ATankBullet>(Location, Rotation, SpawnParameters);
		if (Bullet)
		{
			Bullet->SetInstigator(this);
			Bullet->SetActorScale3D(Bullet->GetActorScale() * sizeOfBullet);
			Bullet->GetComponentByClass<UStaticMeshComponent>()->SetStaticMesh(BulletMesh);	
		}
		
		TankInputs.FireInput = false;
	}
}

const FPlayerTankInputs& ATankPawn::GetTankInputs() const
{
	return TankInputs;
}

void ATankPawn::TankGetShoot()
{
	//Rien a mettre pour le moment donc on le détruit
	if(TankParticleExplosion)
	{
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TankParticleExplosion,
			TankBodyMesh,
			NAME_None,
			FVector(this->GetActorLocation()),
			FRotator(0.f),
			EAttachLocation::Type::KeepRelativeOffset,
			true);
	}
	this->Destroy();
}
