// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "enet6/enet.h"
#include "ENetGameModeTest.generated.h"

class UENetPeer;
class UENetHost;
/**
 * 
 */
UCLASS()
class GP_A5_LITTLEPIPOTANK_API AENetGameModeTest : public AGameModeBase
{
	GENERATED_BODY()

	AENetGameModeTest();
	
	virtual void BeginPlay() override;
	
	void ClientBegin();
	void ServerBegin();

	virtual void Tick(float DeltaSeconds) override;

	void ClientTick(float DeltaSeconds);
	void ServerTick(float DeltaSeconds);
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool isClient = true;

	UPROPERTY()
	TObjectPtr<UENetHost> Host;
	UPROPERTY()
	TObjectPtr<UENetPeer> Peer;

	ENetHost* NetHost;
	ENetPeer* NetPeer;
};
