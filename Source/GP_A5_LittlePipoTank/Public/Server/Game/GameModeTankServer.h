// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Shared/TankGamePhase.h"
#include "GameModeTankServer.generated.h"

/**
 * 
 */
UCLASS()
class GP_A5_LITTLEPIPOTANK_API AGameModeTankServer : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	void UpdateCheckTickPhysics(float DeltaTime);
	void UpdateCheckTickNetwork(float DeltaTime);
	
	void InitGameServer();

	void GamePhysicsTick(float DeltaTime);
	void GameNetworkTick(float DeltaTime);
	
	UFUNCTION(BlueprintCallable)
	void SetGamePhase(ETankGamePhase GamePhase);

	UFUNCTION(BlueprintCallable)
	void NextGamePhase();


	UFUNCTION(BlueprintImplementableEvent)
	void ReactChangeGamePhase(ETankGamePhase InGamePhase);	// Blueprint implementable to handle timer in BP
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ETankGamePhase CurrentTankGamePhase = ETankGamePhase::NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int PlayerCount = 0;

	bool IsServerInitialized = false;

private:
	void PlayerJoined();

	void PlayerLeft();

	float TickDelayPhysics = 1/60.0f;

	float TickDelayNetwork = 1/30.f;

	float CurrentAccumulatedPhysicsTickTime = 0.f;
	float CurrentAccumulatedNetworkTickTime = 0.f;
	
};
