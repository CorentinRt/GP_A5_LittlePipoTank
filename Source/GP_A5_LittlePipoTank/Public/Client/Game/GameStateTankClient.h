// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerDataClient.h"
#include "Shared/NetworkProtocol.h"
#include "Shared/TankGamePhase.h"
#include "Shared/ENetWrapper/ENetPeer.h"
#include "Shared/Game/PlayerTankInputs.h"
#include "GameStateTankClient.generated.h"

/**
 * 
 */
USTRUCT()
struct GP_A5_LITTLEPIPOTANK_API FInterpolationSnapshot
{
	GENERATED_BODY()

public:
	TArray<FPlayersStatePacket::PlayerStateData> PlayerStates;
};

USTRUCT()
struct GP_A5_LITTLEPIPOTANK_API FPredictionSnapshot
{
	GENERATED_BODY()

public:
	UINT8 PredictionIndex;
	FPlayerTankInputs Inputs;
	FVector2D Location;
	float Rotation;
	float AimRotation;
	FVector2D Velocity;
};

USTRUCT(BlueprintType)
struct GP_A5_LITTLEPIPOTANK_API FGameStateTankClient
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	ETankGamePhase CurrentGamePhase = ETankGamePhase::NONE;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TArray<FPlayerDataClient> Players;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	FPlayerTankInputs OwnPlayerInputs;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	int OwnPlayerIndex = 0;

	ENetPeer* ServerPeer;

	// Client Prediction and Reconciliation
	TArray<FPredictionSnapshot> Predictions;
	UINT8 NextPredictionIndex = 0;
	
	// Game Interpolation
	TArray<FInterpolationSnapshot> PlayersStateSnapshots;
	float SnapshotBufferAccumulator = 0.0f;
	UINT8 SnapshotBufferTargetSize = 4;
};
