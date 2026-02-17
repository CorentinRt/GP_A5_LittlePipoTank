// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENetGameMode.h"
#include "enet6/enet.h"
#include "Shared/Game/GameModeTankShared.h"
#include "ENetClientGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GP_A5_LITTLEPIPOTANK_API AENetClientGameMode : public AGameModeTankShared
{
	GENERATED_BODY()

protected:
	virtual void Tick(float DeltaSeconds) override;

public:
	virtual void InitializeNetwork() override;
	virtual void FinalizeNetwork() override;
	
	virtual void RunNetwork() override;

protected:
	virtual void HandleMessage(const OpCode& OpCode, const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset) override;
	
	ENetPeer* ServerPeer;
};
