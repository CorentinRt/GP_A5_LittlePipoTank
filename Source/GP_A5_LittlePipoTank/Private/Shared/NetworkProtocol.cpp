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
	// Size array
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, PlayersData.Num());

	for (const PlayerData& LocalPlayerData : PlayersData)
	{
		// Index
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, LocalPlayerData.Index);
		// Location
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, LocalPlayerData.Location.X);
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, LocalPlayerData.Location.Y);
		// Rotation
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, LocalPlayerData.Rotation);
		// Velocity
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, LocalPlayerData.Velocity.X);
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, LocalPlayerData.Velocity.Y);
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
		
		FVector2D PlayerVelocity = FVector2D::ZeroVector;
		PlayerVelocity.X = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
		PlayerVelocity.Y = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
		
		PlayerData NewPlayerData
		{
			.Index = PlayerIndex,
			.Location = PlayerLocation,
			.Rotation = PlayerRotation,
			.Velocity = PlayerVelocity
		};
		
		PlayersData.Add(MoveTemp(NewPlayerData));
	}
}

void FPlayersVisualPacket::Serialize(TArray<BYTE>& ByteArray) const
{
	// Size Array
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, PlayersVisualData.Num());

	for (const PlayerVisualData& LocalPlayerVisualData : PlayersVisualData)
	{
		// Index
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, LocalPlayerVisualData.PlayerIndex);
		// Location
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, LocalPlayerVisualData.PlayerLocation.X);
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, LocalPlayerVisualData.PlayerLocation.Y);
		// Rotation
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, LocalPlayerVisualData.PlayerRotation);
	}
}

void FPlayersVisualPacket::Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset)
{
	TArray<BYTE>::SizeType PlayersDataSize = UNetworkProtocolHelpers::DeserializeArithmetic<TArray<BYTE>::SizeType>(ByteArray, Offset);

	for (int i = 0; i < PlayersDataSize; ++i)
	{
		int PlayerIndex = UNetworkProtocolHelpers::DeserializeArithmetic<int>(ByteArray, Offset);

		FVector2D PlayerLocation = FVector2D::ZeroVector;
		PlayerLocation.X = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
		PlayerLocation.Y = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);

		float PlayerRotation = UNetworkProtocolHelpers::DeserializeArithmetic<float>(ByteArray, Offset);
		
		PlayerVisualData NewPlayerVisualData
		{
			.PlayerIndex = PlayerIndex,
			.PlayerLocation = PlayerLocation,
			.PlayerRotation = PlayerRotation
		};
		
		PlayersVisualData.Add(MoveTemp(NewPlayerVisualData));
	}
}

void FBulletsStatePacket::Serialize(TArray<BYTE>& ByteArray) const
{
	// Size Array
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, BulletsStateData.Num());

	for (const BulletStateData& BulletStateData : BulletsStateData)
	{
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, BulletStateData.Location.X);
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, BulletStateData.Location.Y);
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, BulletStateData.Rotation);
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, BulletStateData.Destroyed);
	}

	
}

void FBulletsStatePacket::Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset)
{
	TArray<BYTE>::SizeType BulletsDataSize = UNetworkProtocolHelpers::DeserializeArithmetic<TArray<BYTE>::SizeType>(ByteArray, Offset);

	for (int i = 0; i < BulletsDataSize; ++i)
	{
		FVector2D BulletLocation = FVector2D::ZeroVector;
		BulletLocation.X = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
		BulletLocation.Y = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
		
		float BulletRotation = UNetworkProtocolHelpers::DeserializeArithmetic<float>(ByteArray, Offset);
		bool BulletDestroyed = UNetworkProtocolHelpers::DeserializeArithmetic<bool>(ByteArray, Offset);
	}
}

void FGamePhasePacket::Serialize(TArray<BYTE>& ByteArray) const
{
	uint8 GamePhaseByte = static_cast<uint8>(GamePhase);
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, GamePhaseByte);
}

void FGamePhasePacket::Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset)
{
	uint8 GamePhaseByte = UNetworkProtocolHelpers::DeserializeArithmetic<uint8>(ByteArray, Offset);
	GamePhase = static_cast<ETankGamePhase>(GamePhaseByte);
}
