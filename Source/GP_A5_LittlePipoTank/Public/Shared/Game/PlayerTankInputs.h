// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PlayerTankInputs.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct GP_A5_LITTLEPIPOTANK_API FPlayerTankInputs
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category= "PlayerTankInputStruct")
	FVector2D MoveInput = FVector2D::ZeroVector;
	
	UPROPERTY(EditAnywhere, Category= "PlayerTankInputStruct")
	FVector2D AimInput = FVector2D::ZeroVector;
	
	UPROPERTY(EditAnywhere, Category= "PlayerTankInputStruct")
	bool FireInput = false;

	UINT8 InputIndex = 0;
};
