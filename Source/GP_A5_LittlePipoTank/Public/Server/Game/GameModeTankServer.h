// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameStateTankServer.h"
#include "GameFramework/GameModeBase.h"
#include "GameModeTankServer.generated.h"

class IGamePhaseListener;
/**
 * 
 */
UCLASS()
class GP_A5_LITTLEPIPOTANK_API AGameModeTankServer : public AGameModeBase
{
	GENERATED_BODY()

public:

	AGameModeTankServer();
	
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


	void ReactChangeGamePhase(ETankGamePhase InGamePhase);
	
	UFUNCTION(BlueprintImplementableEvent)
	void ReactChangeGamePhase_Implementation(ETankGamePhase InGamePhase);	// Blueprint implementable to handle timer in BP
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FGameStateTankServer GameStateServer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int PlayerCount = 0;

	bool IsServerInitialized = false;

	UFUNCTION(BlueprintCallable)
	void RegisterListener(AActor* InGamePhaseListener);
	UFUNCTION(BlueprintCallable)
	void UnregisterListener(AActor* InGamePhaseListener);

private:
	void PlayerJoined();

	void PlayerLeft();
	
	float TickDelayPhysics = 1/30.0f;

	float TickDelayNetwork = 1/10.f;

	float CurrentAccumulatedPhysicsTickTime = 0.f;
	float CurrentAccumulatedNetworkTickTime = 0.f;

	UPROPERTY()
	TArray<AActor*> GamePhaseListeners;
};
