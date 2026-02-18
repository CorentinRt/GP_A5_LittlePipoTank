// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/NetworkProtocol.h"

#include "Sections/MovieScene3DPathSection.h"
#include "Shared/NetworkProtocolHelpers.h"

void FExemplePacket::Serialize(TArray<BYTE>& ByteArray) const
{
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, X);
}

void FExemplePacket::Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset)
{
	X = UNetworkProtocolHelpers::DeserializeArithmetic<float>(ByteArray, Offset);
}

void FPlayerNamePacket::Serialize(TArray<BYTE>& ByteArray) const
{
	UNetworkProtocolHelpers::SerializeString(ByteArray, Name);
}

void FPlayerNamePacket::Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset)
{
	Name = UNetworkProtocolHelpers::DeserializeString(ByteArray, Offset);
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

void FPlayersStatePacket::Serialize(TArray<BYTE>& ByteArray) const
{
	// Size Array
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OtherPlayersStateData.Num());

	for (const PlayerStateData& OtherPlayerStateData : OtherPlayersStateData)
	{
		// Index
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OtherPlayerStateData.Index);
		// Location
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OtherPlayerStateData.Location.X);
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OtherPlayerStateData.Location.Y);
		// Rotation
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OtherPlayerStateData.Rotation);
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OtherPlayerStateData.AimRotation);
	}

	// Own Player Data
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OwnPlayerData.Index);
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OwnPlayerData.Location.X);
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OwnPlayerData.Location.Y);
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OwnPlayerData.Rotation);
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OwnPlayerData.AimRotation);
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OwnPlayerData.Velocity.X);
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OwnPlayerData.Velocity.Y);
}

void FPlayersStatePacket::Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset)
{
	TArray<BYTE>::SizeType PlayersDataSize = UNetworkProtocolHelpers::DeserializeArithmetic<TArray<BYTE>::SizeType>(ByteArray, Offset);

	for (int i = 0; i < PlayersDataSize; ++i)
	{
		int OtherPlayerIndex = UNetworkProtocolHelpers::DeserializeArithmetic<int>(ByteArray, Offset);

		FVector2D OtherPlayerLocation = FVector2D::ZeroVector;
		OtherPlayerLocation.X = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
		OtherPlayerLocation.Y = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);

		float OtherPlayerRotation = UNetworkProtocolHelpers::DeserializeArithmetic<float>(ByteArray, Offset);
		float OtherPlayerAimRotation = UNetworkProtocolHelpers::DeserializeArithmetic<float>(ByteArray, Offset);
		
		PlayerStateData NewOtherPlayerVisualData
		{
			.Index = OtherPlayerIndex,
			.Location = OtherPlayerLocation,
			.Rotation = OtherPlayerRotation,
			.AimRotation = OtherPlayerAimRotation
		};
		
		OtherPlayersStateData.Add(MoveTemp(NewOtherPlayerVisualData));
	}

	// Own Player Data
	int OwnPlayerIndex = UNetworkProtocolHelpers::DeserializeArithmetic<int>(ByteArray, Offset);

	FVector2D OwnPlayerLocation = FVector2D::ZeroVector;
	OwnPlayerLocation.X = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
	OwnPlayerLocation.Y = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);

	float OwnPlayerRotation = UNetworkProtocolHelpers::DeserializeArithmetic<float>(ByteArray, Offset);
	float OwnPlayerAimRotation = UNetworkProtocolHelpers::DeserializeArithmetic<float>(ByteArray, Offset);

	FVector2D OwnPlayerVelocity = FVector2D::ZeroVector;
	OwnPlayerVelocity.X = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
	OwnPlayerVelocity.Y = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
	
	OwnPlayerData =
	{
		.Index = OwnPlayerIndex,
		.Location = OwnPlayerLocation,
		.Rotation = OwnPlayerRotation,
		.AimRotation = OwnPlayerAimRotation,
		.Velocity = OwnPlayerVelocity
	};
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

void FPlayerInputsPacket::Serialize(TArray<BYTE>& ByteArray) const
{
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, PlayerInputs.MoveInput.X);
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, PlayerInputs.MoveInput.Y);
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, PlayerInputs.AimInput.X);
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, PlayerInputs.AimInput.Y);
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, PlayerInputs.FireInput);
}

void FPlayerInputsPacket::Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset)
{
	PlayerInputs.MoveInput.X = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
	PlayerInputs.MoveInput.Y = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
	PlayerInputs.AimInput.X = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
	PlayerInputs.AimInput.Y = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
	bool ByteFireInput = UNetworkProtocolHelpers::DeserializeArithmetic<bool>(ByteArray, Offset);
}
