// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/NetworkProtocol.h"

#include "Shared/NetworkProtocolHelpers.h"

void FExemplePacket::Serialize(TArray<BYTE>& ByteArray) const
{
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, X);
}

void FExemplePacket::Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset)
{
	X = UNetworkProtocolHelpers::DeserializeArithmetic<float>(ByteArray, Offset);
}
