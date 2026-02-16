// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/TankGamePhase.h"
#include "Shared/Game/PlayerTankInputs.h"
#include "Shared/Game/PlayerData.h"
#include "GameStateTankClient.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct GP_A5_LITTLEPIPOTANK_API FGameStateTankClient
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	ETankGamePhase CurrentGamePhase = ETankGamePhase::NONE;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TArray<FPlayerData> Players;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	FPlayerTankInputs OwnPlayerInputs;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	int OwnPlayerIndex;
};
