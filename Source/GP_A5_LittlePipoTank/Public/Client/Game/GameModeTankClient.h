// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameStateTankClient.h"
#include "Shared/ENetWrapper/ENetClientGameMode.h"
#include "Shared/Game/GameModeTankShared.h"
#include "GameModeTankClient.generated.h"

/**
 * 
 */
UCLASS()
class GP_A5_LITTLEPIPOTANK_API AGameModeTankClient : public AENetClientGameMode
{
	GENERATED_BODY()

public:
	
	AGameModeTankClient();
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	void InitGameClient();

	virtual void GamePhysicsTick(float DeltaTime) override;
	virtual void GameNetworkTick(float DeltaTime) override;

	virtual ETankGamePhase GetCurrentGamePhase() override;

	UFUNCTION(BlueprintCallable)
	void SetClientGamePhase(ETankGamePhase NewGamePhase);
	
	FGameStateTankClient GameStateClient;

	void ReceivePlayerJoinedGame();

	void ReceivePlayerLeaveGame();
	
protected:

	FString GetServerAdressIp() const;

	virtual void HandleMessage(const OpCode& OpCode, const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset) override;

	virtual void HandleConnection(const ENetEvent& event) override;

	virtual void HandleDisconnection(const ENetEvent& event) override;

private:
	void SendClientInputs() const;
	void InterpolateGame(float DeltaTime);
};
