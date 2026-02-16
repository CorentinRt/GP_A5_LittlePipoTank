// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/TankGamePhase.h"
#include "Shared/Game/PlayerData.h"
#include "UObject/Object.h"
#include "GameStateTankServer.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct GP_A5_LITTLEPIPOTANK_API FGameStateTankServer
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	ETankGamePhase CurrentGamePhase = ETankGamePhase::NONE;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TArray<FPlayerData> Players;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int NextPlayerIndex = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int PlayerCount = 0;
};
