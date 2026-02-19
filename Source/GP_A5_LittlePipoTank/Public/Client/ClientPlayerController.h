// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "Shared/Game/PlayerTankInputs.h"
#include "ClientPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;

/**
 * 
 */
UCLASS()
class GP_A5_LITTLEPIPOTANK_API AClientPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	
	virtual void SetupInputComponent() override;

	FPlayerTankInputs& GetTankInputs() ;
	const FPlayerTankInputs& GetTankInputs() const;
	
protected:
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TObjectPtr<UInputMappingContext> DefaultMappingContexts;

	UPROPERTY(EditAnywhere, Category = "Input|Input Actions")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category = "Input|Input Actions")
	UInputAction* AimAction;
	UPROPERTY(EditAnywhere, Category = "Input|Input Actions")
	UInputAction* ShootAction;

	void OnMove(const FInputActionValue& InputActionValue);
	void OnAim(const FInputActionValue& InputActionValue);
	void OnShoot(const FInputActionValue& InputActionValue);

private:
	UPROPERTY(VisibleAnywhere)
	FPlayerTankInputs TankInputs;
};
