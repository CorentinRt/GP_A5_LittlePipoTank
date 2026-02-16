// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "enet6/enet.h"
#include "ENetServerGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GP_A5_LITTLEPIPOTANK_API AENetServerGameMode : public AGameModeBase
{
	GENERATED_BODY()

private:
	ENetHost* ServerHost;
};
