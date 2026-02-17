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
	S_ExempleCode,
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
