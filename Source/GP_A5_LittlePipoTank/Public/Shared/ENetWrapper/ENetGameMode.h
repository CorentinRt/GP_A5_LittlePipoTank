// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "enet6/enet.h"
#include "GameFramework/GameModeBase.h"
#include "ENetGameMode.generated.h"

/**
 * 
 */
enum  class OpCode: UINT8;

UCLASS()
class GP_A5_LITTLEPIPOTANK_API AENetGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	AENetGameMode();
	
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	
public:
	virtual void InitializeNetwork();
	virtual void FinalizeNetwork();
	
	virtual void RunNetwork();

protected:
	virtual void HandleConnection(const ENetEvent& event);
	virtual void HandleDisconnection(const ENetEvent& event);
	virtual void HandleTimeout(const ENetEvent& event);
	virtual void HandleMessage(const OpCode& OpCode, const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset);

private:
	void OnNetworkEventConnect(const ENetEvent& event);
	void OnNetworkEventDisconnect(const ENetEvent& event);
	void OnNetworkEventDisconnectTimeout(const ENetEvent& event);
	void OnNetworkEventReceive(const ENetEvent& event);
	
protected:
	ENetHost* Host;
};
