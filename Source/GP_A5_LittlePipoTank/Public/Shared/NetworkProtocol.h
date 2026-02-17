// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NetworkProtocol.generated.h"

/**
 * 
 */

UENUM()
enum class OpCode : UINT8
{
	S_ExempleCode = 0,
	S_PlayerJoined = 1,
	S_PlayerLeft = 2,
	S_PlayerName = 3,
	S_OwnPlayerState = 4,
	S_PlayerVisual = 5,
	S_BulletsState = 6,
	S_GamePhase = 7,

	C_PlayerInputs = 126,
	C_PlayerName = 127,
	
	
};

USTRUCT()
struct FExemplePacket
{
	GENERATED_BODY()

	~FExemplePacket() = default;

	static constexpr OpCode OpCode = OpCode::S_ExempleCode;
	
	float X;

	void Serialize(TArray<BYTE>& ByteArray) const;
	void Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset);
};

UCLASS()
class GP_A5_LITTLEPIPOTANK_API UNetworkProtocol : public UObject
{
	GENERATED_BODY()
};
