// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/Game/PlayerTankInputs.h"
#include "UObject/Object.h"
#include "PlayerDataClient.generated.h"

class AClientTankBullet;
class AClientTankPawn;
/**
 * 
 */
USTRUCT(BlueprintType)
struct GP_A5_LITTLEPIPOTANK_API FPlayerDataClient
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	int PlayerIndex = 0;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	FString PlayerName = "NULL_NAME";

	UPROPERTY()
	TObjectPtr<AClientTankPawn> Tank;
};

USTRUCT(BlueprintType)
struct GP_A5_LITTLEPIPOTANK_API FBulletDataClient
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	int BulletIndex = 0;

	UPROPERTY()
	TObjectPtr<AClientTankBullet> Bullet;
};
