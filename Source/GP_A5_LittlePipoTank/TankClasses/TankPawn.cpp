#include "TankClasses/TankPawn.h"

#include "AIHelpers.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "TankBullet.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Server/Game/GameModeTankServer.h"

// Sets default values
ATankPawn::ATankPawn()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	TankBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyOfTank"));
	TankBodyMesh->SetupAttachment(GetRootComponent());
	TankHeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadOfTank"));
	TankHeadMesh->SetupAttachment(TankBodyMesh);
	TankHeadMesh->SetUsingAbsoluteRotation(true);
}

// Called when the game starts or when spawned
void ATankPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATankPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator CurrentWorldRot = TankHeadMesh->GetComponentRotation();
	FRotator SmoothedRot = FMath::RInterpTo(
		CurrentWorldRot,
		TargetWorldRotation,
		DeltaTime,
		HeadRotationSpeed
	);
	
	TankHeadMesh->SetWorldRotation(SmoothedRot);
}

// Called to bind functionality to input
void ATankPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller)) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}


	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATankPawn::Move);
		Input->BindAction(AimAction, ETriggerEvent::Triggered, this, &ATankPawn::Aim);
		Input->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ATankPawn::Shoot);
	}
}

void ATankPawn::Move(const FInputActionValue& Value) {
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (IsValid(Controller)) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddControllerYawInput(MovementVector.X);
	}
}

void ATankPawn::Aim(const FInputActionValue& Value)
{
	FVector2D AimVector = Value.Get<FVector2D>();
	if (IsValid(Controller))
	{
		FVector AimFVector {AimVector.X, AimVector.Y, 0.0f};
		TargetWorldRotation = FRotationMatrix::MakeFromX(AimFVector).Rotator();
	}
}

void ATankPawn::Shoot(const FInputActionValue& Value)
{
	bool shootValue = Value.Get<bool>();
	if (IsValid(Controller) && shootValue == true)
	{
		FVector Location(this->GetTransform().GetLocation());
		FRotator Rotation(this->GetActorRotation());
		FActorSpawnParameters SpawnParameters;
		AActor* Bullet = GetWorld()->SpawnActor<ATankBullet>(Location, Rotation, SpawnParameters);
	}
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
}
