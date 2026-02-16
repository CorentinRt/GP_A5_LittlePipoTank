// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Shared/TankGamePhase.h"
#include "GameModeTankShared.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class GP_A5_LITTLEPIPOTANK_API AGameModeTankShared : public AGameModeBase
{
	GENERATED_BODY()

public:

	AGameModeTankShared();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void UpdateCheckTickPhysics(float DeltaTime);
	virtual void UpdateCheckTickNetwork(float DeltaTime);

	virtual void GamePhysicsTick(float DeltaTime);
	virtual void GameNetworkTick(float DeltaTime);
	
	UFUNCTION(BlueprintCallable)
	virtual ETankGamePhase GetCurrentGamePhase();

	virtual void ReactChangeGamePhase(ETankGamePhase InGamePhase);

	UFUNCTION(BlueprintImplementableEvent)
	void ReactChangeGamePhase_Implementation(ETankGamePhase InGamePhase);	// Blueprint implementable to handle timer in BP

	UFUNCTION(BlueprintCallable)
	void RegisterListener(AActor* InGamePhaseListener);
	UFUNCTION(BlueprintCallable)
	void UnregisterListener(AActor* InGamePhaseListener);

	UFUNCTION(BlueprintCallable)
	void RegisterPhysicsTickable(AActor* InPhysicsTickable);
	UFUNCTION(BlueprintCallable)
	void UnregisterPhysicsTickable(AActor* InPhysicsTickable);


protected:
	virtual void SetGamePhase(ETankGamePhase& CurrentGamePhase, ETankGamePhase NewGamePhase);
	
	float TickDelayPhysics = 1/30.0f;
	float TickDelayNetwork = 1/10.f;

	float CurrentAccumulatedPhysicsTickTime = 0.f;
	float CurrentAccumulatedNetworkTickTime = 0.f;

	UPROPERTY()
	TArray<AActor*> GamePhaseListeners;

	UPROPERTY()
	TArray<AActor*> PhysicsTickables;
};
