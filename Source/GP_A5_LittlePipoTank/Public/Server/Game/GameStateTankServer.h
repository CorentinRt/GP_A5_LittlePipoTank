// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/TankGamePhase.h"
#include "Server/Game/PlayerDataServer.h"
#include "GameStateTankServer.generated.h"

class ATankBullet;

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
	TArray<FPlayerDataServer> Players;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int NextPlayerIndex = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int PlayerCount = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<TObjectPtr<ATankBullet>> AllTankBullets;

	UPROPERTY()
	int NextBulletIndex = 0;
};
