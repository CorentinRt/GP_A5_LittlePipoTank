// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/ENetWrapper/ENetPeer.h"
#include "Shared/Game/PlayerTankInputs.h"
//#include "TankClasses/TankPawn.h"
#include "PlayerDataServer.generated.h"

/**
 * 
 */

class ATankPawn;

USTRUCT(BlueprintType)
struct GP_A5_LITTLEPIPOTANK_API FPlayerDataServer
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	int PlayerIndex = 0;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	FString PlayerName = "NULL_NAME";

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	FPlayerTankInputs PlayerInputs;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<ATankPawn> PlayerTanks = nullptr;
	
	ENetPeer* Peer = nullptr;
	
};
