#include "TankClasses/TankPawn.h"

#include "AIHelpers.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GP_A5_LittlePipoTank.h"
#include "TankBullet.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Server/Game/GameModeTankServer.h"

// Sets default values
ATankPawn::ATankPawn()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TankBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	TankBoxCollision->SetupAttachment(GetRootComponent());

	TankBodyMeshHolder = CreateDefaultSubobject<USceneComponent>(TEXT("TankBodyMeshHolder"));
	TankBodyMeshHolder->SetupAttachment(TankBoxCollision);
	
	//Body
	TankBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyOfTank"));
	TankBodyMesh->SetupAttachment(TankBodyMeshHolder);

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

}

void ATankPawn::Destroyed()
{
	Super::Destroyed();
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
	// UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Fire iNput {0}", TankInputs.FireInput);
}

void ATankPawn::MoveTank(float MoveInput, float DeltaTime, bool UseSweep)
{
	FVector Dir = GetActorForwardVector();

	FVector LastActorLocation = GetActorLocation();

	SetActorLocation(LastActorLocation + Speed * Dir * MoveInput * DeltaTime, UseSweep);
}

void ATankPawn::RotateTank(float RotateInput, float DeltaTime)
{
	FRotator RotationDelta = {0.f, RotateInput, 0.f};
	AddActorLocalRotation(RotationDelta * DeltaTime * RotateSpeed);
}

bool ATankPawn::CanShoot() const
{
	if (DelayBeforeShootingAvailable <= 0.f)
		return true;

	return false;
}

void ATankPawn::UpdateShootCooldown(float DeltaTime)
{
	if (DelayBeforeShootingAvailable > 0.f)
	{
		DelayBeforeShootingAvailable -= DeltaTime;
	}
}

float ATankPawn::GetHeadAimRotation() const
{
	return TankHeadMesh->GetComponentRotation().Yaw;
}

float ATankPawn::GetTankRotation() const
{
	return GetActorRotation().Yaw;
}

FVector2D ATankPawn::GetTankLocation() const
{
	return {GetActorLocation().X, GetActorLocation().Y};
}

void ATankPawn::UpdatePhysics(float DeltaTime, bool UseSweep)
{
	UpdateShootCooldown(DeltaTime);
	
	if (IsHidden())
		return;
	
	// UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Tick physi tank");
	//Move
	MoveTank(bBlockAllInputs ? 0.f : TankInputs.MoveInput.Y, DeltaTime, UseSweep);

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

	//Fire Input

	if (bBlockAllInputs)
		TankInputs.FireInput = false;
	
	if(TankInputs.FireInput == true)
	{
		if (CanShoot())
		{
			DelayBeforeShootingAvailable = CooldownBetweenEachShoot;
			
			FVector Location(TankShootingPoint->GetComponentLocation());
			FRotator Rotation(TankShootingPoint->GetComponentRotation());
			Rotation.Yaw += 90;
			FActorSpawnParameters SpawnParameters;
			
			if (ATankBullet* Bullet = GetWorld()->SpawnActor<ATankBullet>(TankBulletClassBlueprint, Location, Rotation, SpawnParameters))
			{
				Bullet->SetInstigator(this);
				//Bullet->SetActorScale3D(Bullet->GetActorScale() * sizeOfBullet);
				//Bullet->GetComponentByClass<UStaticMeshComponent>()->SetStaticMesh(BulletMesh);	
				ReceiveShooting(Bullet);
				
				OnSpawnBullet.Broadcast(Bullet);
			}
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
		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		this,
		TankParticleExplosion,
FVector(this->GetActorLocation()),
FRotator(0.f),
FVector::One(),
true);
	}
	ReceiveGetShoot();
	
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	OnTankDestroyed.Broadcast(this);
}

void ATankPawn::ReactOnGamePhaseChanged_Implementation(ETankGamePhase InGamePhase)
{
	IGamePhaseListener::ReactOnGamePhaseChanged_Implementation(InGamePhase);

	switch (InGamePhase)
	{
	case ETankGamePhase::IN_GAME:
		bBlockAllInputs = false;
		UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Block input false");
		break;
	case ETankGamePhase::WAITING_PLAYER:
		bBlockAllInputs = false;
		UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Block input true");
		break;
	case ETankGamePhase::PRE_GAME:
		bBlockAllInputs = true;
		UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Block input true");
		break;
	case ETankGamePhase::POST_GAME:
		bBlockAllInputs = true;
		UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Block input true");
		break;
	case ETankGamePhase::NONE:
		break;

	default:
		break;
	}
}