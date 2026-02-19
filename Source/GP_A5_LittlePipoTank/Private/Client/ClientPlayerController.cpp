// Fill out your copyright notice in the Description page of Project Settings.


#include "Client/ClientPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInput/Public/InputAction.h"
#include "EnhancedInputSubsystems.h"


void AClientPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
		Subsystem->AddMappingContext(DefaultMappingContexts, 0);
	}

	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(InputComponent)) {
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AClientPlayerController::OnMove);
		Input->BindAction(MoveAction, ETriggerEvent::Completed, this, &AClientPlayerController::OnMove);
		Input->BindAction(AimAction, ETriggerEvent::Triggered, this, &AClientPlayerController::OnAim);
		Input->BindAction(ShootAction, ETriggerEvent::Started, this, &AClientPlayerController::OnShoot);
		Input->BindAction(ShootAction, ETriggerEvent::Completed, this, &AClientPlayerController::OnShoot);
	}
}

const FPlayerTankInputs& AClientPlayerController::GetTankInputs() const
{
	return TankInputs;
}

void AClientPlayerController::OnMove(const FInputActionValue& InputActionValue)
{
	FVector2D MoveVector = InputActionValue.Get<FVector2D>();

	TankInputs.MoveInput = MoveVector;
}

void AClientPlayerController::OnAim(const FInputActionValue& InputActionValue)
{
	FVector2D AimVector = InputActionValue.Get<FVector2D>();

	TankInputs.AimInput = AimVector;
}

void AClientPlayerController::OnShoot(const FInputActionValue& InputActionValue)
{
	bool FireInput = InputActionValue.Get<bool>();
	
	TankInputs.FireInput = FireInput;
}
