// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "enet6/enet.h"
#include "ENetGameModeTest.generated.h"

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

	virtual void Tick(float DeltaSeconds) override;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool isClient = true;

	TObjectPtr<UENetHost> Host;

	ENetHost* NetHost;
	ENetPeer* Peer;
};
