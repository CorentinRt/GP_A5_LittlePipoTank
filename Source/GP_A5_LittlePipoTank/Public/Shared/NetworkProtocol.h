// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TankGamePhase.h"
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
	S_OwnPlayerState = 3,
	S_PlayersVisual = 4,
	S_BulletsState = 5,
	S_GamePhase = 6,

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

USTRUCT()
struct FPlayerJoinedPacket
{
	GENERATED_BODY()

	~FPlayerJoinedPacket() = default;

	static constexpr OpCode OpCode = OpCode::S_PlayerJoined;
	
	int PlayerIndex = -1;

	void Serialize(TArray<BYTE>& ByteArray) const;
	void Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset);
};

USTRUCT()
struct FPlayerLeftPacket
{
	GENERATED_BODY()

	~FPlayerLeftPacket() = default;

	static constexpr OpCode OpCode = OpCode::S_PlayerLeft;
	
	int PlayerIndex = -1;

	void Serialize(TArray<BYTE>& ByteArray) const;
	void Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset);
};

USTRUCT()
struct FOwnPlayerStatePacket
{
	GENERATED_BODY()

	~FOwnPlayerStatePacket() = default;

	static constexpr OpCode OpCode = OpCode::S_OwnPlayerState;
	
	struct PlayerData
	{
		int Index = -1;
		FVector2D Location = FVector2D::ZeroVector;
		float Rotation = 0.f;
		float AimRotation = 0.f;
		FVector2D Velocity = FVector2D::ZeroVector;
	};

	TArray<PlayerData> PlayersData;

	void Serialize(TArray<BYTE>& ByteArray) const;
	void Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset);
};

USTRUCT()
struct FPlayersVisualPacket
{
	GENERATED_BODY()

	~FPlayersVisualPacket() = default;

	static constexpr OpCode OpCode = OpCode::S_PlayersVisual;

	struct PlayerVisualData
	{
		int PlayerIndex = -1;
		FVector2D PlayerLocation = FVector2D::ZeroVector;
		float PlayerRotation = 0.f;
		float AimRotation = 0.f;
	};

	TArray<PlayerVisualData> PlayersVisualData;

	void Serialize(TArray<BYTE>& ByteArray) const;
	void Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset);
};

USTRUCT()
struct FBulletsStatePacket
{
	GENERATED_BODY()

	~FBulletsStatePacket() = default;

	static constexpr OpCode OpCode = OpCode::S_BulletsState;
	
	struct BulletStateData
	{
		FVector2D Location = FVector2D::ZeroVector;
		float Rotation = 0.f;
		bool Destroyed = false;
	};

	TArray<BulletStateData> BulletsStateData;

	void Serialize(TArray<BYTE>& ByteArray) const;
	void Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset);
};

USTRUCT()
struct FGamePhasePacket
{
	GENERATED_BODY()

	~FGamePhasePacket() = default;

	static constexpr OpCode OpCode = OpCode::S_GamePhase;

	ETankGamePhase GamePhase = ETankGamePhase::NONE;

	void Serialize(TArray<BYTE>& ByteArray) const;
	void Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset);
};

UCLASS()
class GP_A5_LITTLEPIPOTANK_API UNetworkProtocol : public UObject
{
	GENERATED_BODY()
};
