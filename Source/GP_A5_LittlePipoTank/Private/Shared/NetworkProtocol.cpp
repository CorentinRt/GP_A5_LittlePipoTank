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

void FPlayerJoinedPacket::Serialize(TArray<BYTE>& ByteArray) const
{
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, PlayerIndex);
}

void FPlayerJoinedPacket::Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset)
{
	PlayerIndex = UNetworkProtocolHelpers::DeserializeArithmetic<int>(ByteArray, Offset);
}

void FPlayerLeftPacket::Serialize(TArray<BYTE>& ByteArray) const
{
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, PlayerIndex);
}

void FPlayerLeftPacket::Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset)
{
	PlayerIndex = UNetworkProtocolHelpers::DeserializeArithmetic<int>(ByteArray, Offset);
}

void FOwnPlayerStatePacket::Serialize(TArray<BYTE>& ByteArray) const
{
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, PlayersData.Num());

	for (const PlayerData& LocalPlayerData : PlayersData)
	{
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, LocalPlayerData.PlayerIndex);
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, LocalPlayerData.PlayerLocation.X);
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, LocalPlayerData.PlayerLocation.Y);
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, LocalPlayerData.PlayerRotation);
	}
}

void FOwnPlayerStatePacket::Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset)
{
	TArray<BYTE>::SizeType PlayersDataSize = UNetworkProtocolHelpers::DeserializeArithmetic<TArray<BYTE>::SizeType>(ByteArray, Offset);

	for (int i = 0; i < PlayersDataSize; ++i)
	{
		int PlayerIndex = UNetworkProtocolHelpers::DeserializeArithmetic<int>(ByteArray, Offset);

		FVector2D PlayerLocation = FVector2D::ZeroVector;
		PlayerLocation.X = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
		PlayerLocation.Y = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);

		float PlayerRotation = UNetworkProtocolHelpers::DeserializeArithmetic<float>(ByteArray, Offset);
		
		PlayerData NewPlayerData
		{
			.PlayerIndex = PlayerIndex,
			.PlayerLocation = PlayerLocation,
			.PlayerRotation = PlayerRotation
		};
		
		PlayersData.Add(MoveTemp(NewPlayerData));
	}
}
