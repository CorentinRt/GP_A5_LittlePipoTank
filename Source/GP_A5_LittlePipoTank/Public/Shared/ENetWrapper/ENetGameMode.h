// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "enet6/enet.h"
#include "GameFramework/GameModeBase.h"
#include "ENetGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GP_A5_LITTLEPIPOTANK_API AENetGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	AENetGameMode();
	
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	
public:
	virtual void InitializeNetwork();
	virtual void FinalizeNetwork();
	
	virtual void RunNetwork();
	
protected:
	ENetHost* Host;
};
