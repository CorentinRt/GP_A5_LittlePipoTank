// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerTankInputs.h"
#include "UObject/Object.h"
#include "PlayerData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct GP_A5_LITTLEPIPOTANK_API FPlayerData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	int PlayerIndex;

	UPROPERTY(BlueprintReadWrite)
	FString PlayerName = "NULL_NAME";

	UPROPERTY(BlueprintReadWrite)
	FPlayerTankInputs PlayerInputs;
	
};
