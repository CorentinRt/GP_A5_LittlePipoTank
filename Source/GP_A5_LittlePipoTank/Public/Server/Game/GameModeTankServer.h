// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameStateTankServer.h"
#include "PlayerTankSpawnPoint.h"
#include "Shared/ENetWrapper/ENetServerGameMode.h"
#include "Server/Game/GamePhasesData.h"
#include "Shared/Game/GameModeTankShared.h"
#include "GameModeTankServer.generated.h"

/**
 * 
 */
UCLASS()
class GP_A5_LITTLEPIPOTANK_API AGameModeTankServer : public AENetServerGameMode
{
	GENERATED_BODY()

public:

	AGameModeTankServer();
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	void InitGameServer();

	virtual void GamePhysicsTick(float DeltaTime) override;
	virtual void GameNetworkTick(float DeltaTime) override;

	void SendGameStatePacketToAllClients();
	
	virtual ETankGamePhase GetCurrentGamePhase() override;

	UFUNCTION(BlueprintCallable)
	void SetServerGamePhase(ETankGamePhase NewGamePhase);
	
	UFUNCTION(BlueprintCallable)
	void NextGamePhase();

	void UpdateCurrentGamePhase(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	float GetGamePhaseDuration(ETankGamePhase InGamePhase);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FGameStateTankServer GameStateServer;

	bool IsServerInitialized = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UGamePhasesData> GamePhasesData;

protected:
	virtual void HandleMessage(const OpCode& OpCode, const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset, ENetPeer* Peer) override;
	
	virtual void HandleConnection(const ENetEvent& event) override;

	virtual void HandleDisconnection(const ENetEvent& event) override;

	void GetAllPlayerSpawnPoints();

	UFUNCTION(BlueprintCallable)
	bool SpawnTankPlayer(FPlayerDataServer& InPlayer);

	UPROPERTY(EditAnywhere)
	TSubclassOf<ATankPawn> TankPawnClassBlueprint;
	
private:
	void PlayerJoined(ENetPeer* InPeer, const FString& InPlayerName);

	void PlayerLeft(const ENetEvent& event, int IndexToRemove);
	
	float CurrentAccumulatedGamePhaseTime = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TArray<APlayerTankSpawnPoint*> PlayersSpawnPoints;
};
