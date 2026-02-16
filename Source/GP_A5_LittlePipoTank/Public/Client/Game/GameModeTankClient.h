// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/Game/GameModeTankShared.h"
#include "GameModeTankClient.generated.h"

/**
 * 
 */
UCLASS()
class GP_A5_LITTLEPIPOTANK_API AGameModeTankClient : public AGameModeTankShared
{
	GENERATED_BODY()

public:
	
	AGameModeTankClient();
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	void InitGameClient();

	virtual void GamePhysicsTick(float DeltaTime) override;
	virtual void GameNetworkTick(float DeltaTime) override;
};
