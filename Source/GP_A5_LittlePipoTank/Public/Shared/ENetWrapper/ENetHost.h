// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EENetAddressType.h"
#include "UObject/Object.h"
#include "enet6/enet.h"

#include "ENetHost.generated.h"
/**
 * 
 */

class UENetPeer;
class UENetHost;

USTRUCT()
struct FENetAddress
{
	GENERATED_BODY()

	~FENetAddress() = default;

	bool SetHost(const EENetAddressType& InType, const FString& InAddress);
	void SetPort(UINT16 InPort);

	EENetAddressType GetAddressType() const;
	
	ENetAddress& GetAddressHandle();
	const ENetAddress& GetAddressHandle() const;
	
private:
	ENetAddress Address = {};
};

UCLASS()
class GP_A5_LITTLEPIPOTANK_API UENetHost : public UObject
{
	GENERATED_BODY()

public:
	virtual void BeginDestroy() override;

	void Create(const EENetAddressType& InType, const FENetAddress* InAddress = nullptr, SIZE_T AmountOfPeers = 1);
	void Connect(const FENetAddress& InAddress, UENetPeer& InPeer);

	bool Service(UINT32 timeToWait) const;
	void ServiceAndCheckEvents() const;
	
	const ENetHost* GetHandle() const;
	
private:
	ENetHost* Host;
};
