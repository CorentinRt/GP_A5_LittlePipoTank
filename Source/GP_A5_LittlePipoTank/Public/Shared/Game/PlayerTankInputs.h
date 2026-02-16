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
	UPROPERTY(BlueprintReadWrite)
	float MovementsInputs = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float LookDirInputs = 0.f;
	
	
};
