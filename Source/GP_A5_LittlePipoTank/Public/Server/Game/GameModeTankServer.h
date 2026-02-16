// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GamePhasesData.h"
#include "GameStateTankServer.h"
#include "GameFramework/GameModeBase.h"
#include "Shared/Game/GameModeTankShared.h"
#include "GameModeTankServer.generated.h"

class IGamePhaseListener;
/**
 * 
 */
UCLASS()
class GP_A5_LITTLEPIPOTANK_API AGameModeTankServer : public AGameModeTankShared
{
	GENERATED_BODY()

public:

	AGameModeTankServer();
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	void InitGameServer();

	virtual void GamePhysicsTick(float DeltaTime) override;
	virtual void GameNetworkTick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void NextGamePhase();

	void UpdateCurrentGamePhase(float DeltaTime);

	float GetGamePhaseDuration(ETankGamePhase InGamePhase);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FGameStateTankServer GameStateServer;

	bool IsServerInitialized = false;

	UFUNCTION(BlueprintCallable)
	void RegisterListener(AActor* InGamePhaseListener);
	UFUNCTION(BlueprintCallable)
	void UnregisterListener(AActor* InGamePhaseListener);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UGamePhasesData> GamePhasesData;
	
private:
	void PlayerJoined();

	void PlayerLeft();

	float CurrentAccumulatedGamePhaseTime = 0.f;
};
