// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameStateTankClient.h"
#include "Shared/ENetWrapper/ENetClientGameMode.h"
#include "Shared/Game/GameModeTankShared.h"
#include "GameModeTankClient.generated.h"

class UPlayersTankColorData;
class AClientPlayerController;
class AClientTankPawn;
class AClientTankBullet;
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

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FGameStateTankClient GameStateClient;

	void ReceivePlayerJoinedGame();

	void ReceivePlayerLeaveGame();
	
protected:

	UFUNCTION(BlueprintImplementableEvent)
	void ReceiveOnTankJustExploded();
	
	FString GetServerAdressIp() const;

	virtual void HandleMessage(const OpCode& OpCode, const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset, ENetPeer* Peer) override;

	virtual void HandleConnection(const ENetEvent& event) override;

	virtual void HandleDisconnection(const ENetEvent& event) override;

	virtual void ReactChangeGamePhase(ETankGamePhase InGamePhase) override;
private:
	void InterpolateGame(float DeltaTime);

	void PredictClient(float DeltaTime);
	void ReconciliateClient(const FGameStatePacket::OwnPlayerStateData& OwnPlayerData);

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<AClientTankPawn> BlueprintClientTankClass;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<AClientTankBullet> BlueprintClientBulletClass;
	
	UPROPERTY()
	TObjectPtr<AClientPlayerController> PlayerController;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UPlayersTankColorData> PlayersColorData;
};
