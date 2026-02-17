// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "enet6/enet.h"
#include "ENetPeer.generated.h"

/**
 * 
 */
UCLASS()
class GP_A5_LITTLEPIPOTANK_API UENetPeer : public UObject
{
	GENERATED_BODY()

public:
	void Init(ENetPeer* InNetPeerHandle);
	virtual void BeginDestroy() override;

	bool Send(ENetPacket* Packet) const;
	
	bool IsConnected() const;

	const ENetPeer* GetHandle() const;
	
private:
	ENetPeer* Peer;
};
