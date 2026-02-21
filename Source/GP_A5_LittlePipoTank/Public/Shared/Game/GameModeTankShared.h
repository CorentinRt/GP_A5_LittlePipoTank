// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/TankGamePhase.h"
#include "Shared/ENetWrapper/ENetGameMode.h"
#include "GameModeTankShared.generated.h"

class APlayerTankSpawnPoint;
/**
 * 
 */
UCLASS(BlueprintType)
class GP_A5_LITTLEPIPOTANK_API AGameModeTankShared : public AENetGameMode
{
	GENERATED_BODY()

public:

	AGameModeTankShared();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void AsyncPhysicsTickActor(float DeltaTime, float SimTime) override;
	
	virtual void UpdateCheckTickPhysics(float DeltaTime);
	virtual void UpdateCheckTickNetwork(float DeltaTime);

	virtual void GamePhysicsTick(float DeltaTime);
	virtual void GameNetworkTick(float DeltaTime);
	
	UFUNCTION(BlueprintCallable)
	virtual ETankGamePhase GetCurrentGamePhase();

	virtual void ReactChangeGamePhase(ETankGamePhase InGamePhase);

	UFUNCTION(BlueprintImplementableEvent)
	void ReactChangeGamePhase_Implementation(ETankGamePhase InGamePhase);	// Blueprint implementable to handle timer in BP

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeGamePhase, ETankGamePhase, NewGamePhase);
	FOnChangeGamePhase OnChangeGamePhase;

protected:
	virtual void SetGamePhase(ETankGamePhase& CurrentGamePhase, ETankGamePhase NewGamePhase);

	virtual void GetAllPlayerSpawnPoints();
	
	float TickDelayPhysics = 1/60.0f;
	float TickDelayNetwork = 1/10.f;

	float CurrentAccumulatedPhysicsTickTime = 0.f;
	float CurrentAccumulatedNetworkTickTime = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TArray<APlayerTankSpawnPoint*> PlayersSpawnPoints;
};
