// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENetGameMode.h"
#include "enet6/enet.h"
#include "ENetClientGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GP_A5_LITTLEPIPOTANK_API AENetClientGameMode : public AENetGameMode
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaSeconds) override;

	virtual void InitializeNetwork() override;
	virtual void FinalizeNetwork() override;
	
	virtual void RunNetwork() override;
	
private:
	ENetPeer* ServerPeer;
};
