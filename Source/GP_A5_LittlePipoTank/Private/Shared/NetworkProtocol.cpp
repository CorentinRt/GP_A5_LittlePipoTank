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

void FSpawnTankPacket::Serialize(TArray<BYTE>& ByteArray) const
{
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, TankSpawnsData.Num());

	for (const FSpawnTankPacket::TankSpawnData& LocalTankData : TankSpawnsData)
	{
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, LocalTankData.PlayerIndex);
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, LocalTankData.SpawnLocation.X);
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, LocalTankData.SpawnLocation.Y);
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, LocalTankData.SpawnLocation.Z);
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, LocalTankData.SpawnRotation);
	}
}

void FSpawnTankPacket::Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset)
{
	TArray<BYTE>::SizeType Size = UNetworkProtocolHelpers::DeserializeArithmetic<TArray<BYTE>::SizeType>(ByteArray, Offset);

	for (int i = 0; i < Size; ++i)
	{
		FSpawnTankPacket::TankSpawnData NewTankSpawnData = {};
		NewTankSpawnData.PlayerIndex = UNetworkProtocolHelpers::DeserializeArithmetic<int>(ByteArray, Offset);
		FVector TempSpawnlocation = FVector::ZeroVector;
		TempSpawnlocation.X = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
		TempSpawnlocation.Y = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
		TempSpawnlocation.Z = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
		NewTankSpawnData.SpawnLocation = TempSpawnlocation;

		NewTankSpawnData.SpawnRotation = UNetworkProtocolHelpers::DeserializeArithmetic<float>(ByteArray, Offset);
		
		TankSpawnsData.Add(MoveTemp(NewTankSpawnData));
	}
}

void FDestroyTankPacket::Serialize(TArray<BYTE>& ByteArray) const
{
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, PlayerIndex);
}

void FDestroyTankPacket::Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset)
{
	PlayerIndex = UNetworkProtocolHelpers::DeserializeArithmetic<int>(ByteArray, Offset);
}

void FInitClientDataPacket::Serialize(TArray<BYTE>& ByteArray) const
{
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OwnPlayerIndex);
}

void FInitClientDataPacket::Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset)
{
	OwnPlayerIndex = UNetworkProtocolHelpers::DeserializeArithmetic<int>(ByteArray, Offset);
}

void FPlayerNamePacket::Serialize(TArray<BYTE>& ByteArray) const
{
	UNetworkProtocolHelpers::SerializeString(ByteArray, Name);
}

void FPlayerNamePacket::Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset)
{
	Name = UNetworkProtocolHelpers::DeserializeString(ByteArray, Offset);
}

void FPlayerListPacket::Serialize(TArray<BYTE>& ByteArray) const
{
	// Size
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, Players.Num());

	for (int i = 0; i < Players.Num(); ++i)
	{
		UNetworkProtocolHelpers::SerializeString(ByteArray, Players[i].Name);
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, Players[i].Index);
	}
}

void FPlayerListPacket::Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset)
{
	TArray<BYTE>::SizeType Size = UNetworkProtocolHelpers::DeserializeArithmetic<TArray<BYTE>::SizeType>(ByteArray, Offset);

	for (int i = 0; i < Size; ++i)
	{
		FString Name = UNetworkProtocolHelpers::DeserializeString(ByteArray, Offset);
		int Index = UNetworkProtocolHelpers::DeserializeArithmetic<int>(ByteArray, Offset);
		
		Player NewPlayer
		{
			.Name = Name,
			.Index = Index
		};
		
		Players.Add(MoveTemp(NewPlayer));
	}
}

void FGameStatePacket::Serialize(TArray<BYTE>& ByteArray) const
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

	// Bullet Array
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, BulletsStateData.Num());

	for (const BulletStateData& BulletStateData : BulletsStateData)
	{
		// Index
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, BulletStateData.Index);
		// Location
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, BulletStateData.Location.X);
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, BulletStateData.Location.Y);
		// Rotation
		UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, BulletStateData.Rotation);
	}

	// Own Player Data
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OwnPlayerData.Index);
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OwnPlayerData.Location.X);
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OwnPlayerData.Location.Y);
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OwnPlayerData.Rotation);
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OwnPlayerData.AimRotation);
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OwnPlayerData.Velocity.X);
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OwnPlayerData.Velocity.Y);
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OwnPlayerData.PlayerTankInputs.MoveInput.X);
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OwnPlayerData.PlayerTankInputs.MoveInput.Y);
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OwnPlayerData.PlayerTankInputs.AimInput.X);
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OwnPlayerData.PlayerTankInputs.AimInput.Y);
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OwnPlayerData.PlayerTankInputs.FireInput);
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, OwnPlayerData.PlayerTankInputs.InputIndex);
}

void FGameStatePacket::Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset)
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

	// Bullets Data

	TArray<BYTE>::SizeType BulletsDataSize = UNetworkProtocolHelpers::DeserializeArithmetic<TArray<BYTE>::SizeType>(ByteArray, Offset);
	for (int i = 0; i < BulletsDataSize; ++i)
	{
		int BulletIndex = UNetworkProtocolHelpers::DeserializeArithmetic<int>(ByteArray, Offset);

		FVector2D BulletLocation = FVector2D::ZeroVector;
		BulletLocation.X = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
		BulletLocation.Y = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);

		float BulletRotation = UNetworkProtocolHelpers::DeserializeArithmetic<float>(ByteArray, Offset);
		
		BulletStateData NewBulletVisualData
		{
			.Index = BulletIndex,
			.Location = BulletLocation,
			.Rotation = BulletRotation,
		};
		
		BulletsStateData.Add(MoveTemp(NewBulletVisualData));
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

	FPlayerTankInputs PlayerTankInputs = {};

	PlayerTankInputs.MoveInput.X = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
	PlayerTankInputs.MoveInput.Y = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
	PlayerTankInputs.AimInput.X = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
	PlayerTankInputs.AimInput.Y = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
	PlayerTankInputs.FireInput = UNetworkProtocolHelpers::DeserializeArithmetic<bool>(ByteArray, Offset);
	PlayerTankInputs.InputIndex = UNetworkProtocolHelpers::DeserializeArithmetic<UINT8>(ByteArray, Offset);
	
	OwnPlayerData =
	{
		.Index = OwnPlayerIndex,
		.Location = OwnPlayerLocation,
		.Rotation = OwnPlayerRotation,
		.AimRotation = OwnPlayerAimRotation,
		.Velocity = OwnPlayerVelocity,
		.PlayerTankInputs = PlayerTankInputs
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
	UNetworkProtocolHelpers::SerializeArithmetic(ByteArray, PredictionIndex);
}

void FPlayerInputsPacket::Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset)
{
	PlayerInputs.MoveInput.X = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
	PlayerInputs.MoveInput.Y = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
	PlayerInputs.AimInput.X = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
	PlayerInputs.AimInput.Y = UNetworkProtocolHelpers::DeserializeArithmetic<double>(ByteArray, Offset);
	PlayerInputs.FireInput = UNetworkProtocolHelpers::DeserializeArithmetic<bool>(ByteArray, Offset);
	PredictionIndex = UNetworkProtocolHelpers::DeserializeArithmetic<UINT8>(ByteArray, Offset);
}
