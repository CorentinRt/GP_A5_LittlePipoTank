// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TankGamePhase.h"
#include "Game/PlayerTankInputs.h"
#include "UObject/Object.h"
#include "NetworkProtocol.generated.h"

/**
 * 
 */

UENUM()
enum class OpCode : UINT8
{
	S_ExempleCode = 0,
	S_PlayerList = 1,
	S_GameState = 2,
	S_BulletsState = 3,
	S_GamePhase = 4,
	S_InitClientData = 5,
	S_SpawnTank = 6,
	S_DestroyTank = 7,

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
struct FSpawnTankPacket
{
	GENERATED_BODY()

	~FSpawnTankPacket() = default;

	static constexpr OpCode OpCode = OpCode::S_SpawnTank;
	
	int PlayerIndex = -1;
	
	void Serialize(TArray<BYTE>& ByteArray) const;
	void Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset);
};

USTRUCT()
struct FDestroyTankPacket
{
	GENERATED_BODY()

	~FDestroyTankPacket() = default;

	static constexpr OpCode OpCode = OpCode::S_DestroyTank;
	
	int PlayerIndex = -1;
	
	void Serialize(TArray<BYTE>& ByteArray) const;
	void Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset);
};

USTRUCT()
struct FInitClientDataPacket
{
	GENERATED_BODY()

	~FInitClientDataPacket() = default;

	static constexpr OpCode OpCode = OpCode::S_InitClientData;
	
	int OwnPlayerIndex = -1;

	void Serialize(TArray<BYTE>& ByteArray) const;
	void Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset);
};

USTRUCT()
struct FPlayerNamePacket
{
	GENERATED_BODY()
	
	~FPlayerNamePacket() = default;

	static constexpr OpCode OpCode = OpCode::C_PlayerName;

	FString Name;

	void Serialize(TArray<BYTE>& ByteArray) const;
	void Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset);
};

USTRUCT()
struct FPlayerListPacket
{
	GENERATED_BODY()

	~FPlayerListPacket() = default;

	static constexpr OpCode OpCode = OpCode::S_PlayerList;

	struct Player
	{
		FString Name = "";
		int Index = -1;
	};
	
	TArray<Player> Players;

	void Serialize(TArray<BYTE>& ByteArray) const;
	void Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset);
};

USTRUCT()
struct FGameStatePacket
{
	GENERATED_BODY()

	~FGameStatePacket() = default;

	static constexpr OpCode OpCode = OpCode::S_GameState;

	struct OwnPlayerStateData
	{
		int Index = -1;
		FVector2D Location = FVector2D::ZeroVector;
		float Rotation = 0.f;
		float AimRotation = 0.f;
		FVector2D Velocity = FVector2D::ZeroVector;
	};
	
	struct PlayerStateData
	{
		int Index = -1;
		FVector2D Location = FVector2D::ZeroVector;
		float Rotation = 0.f;
		float AimRotation = 0.f;
	};
	
	struct BulletStateData
	{
		int Index = -1;
		FVector2D Location = FVector2D::ZeroVector;
		float Rotation = 0.f;
	};

	OwnPlayerStateData OwnPlayerData;
	TArray<PlayerStateData> OtherPlayersStateData;
	TArray<BulletStateData> BulletsStateData;
	
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

USTRUCT()
struct FPlayerInputsPacket
{
	GENERATED_BODY()

	~FPlayerInputsPacket() = default;

	static constexpr OpCode OpCode = OpCode::C_PlayerInputs;

	FPlayerTankInputs PlayerInputs;
	UINT8 PredictionIndex = -1;

	void Serialize(TArray<BYTE>& ByteArray) const;
	void Deserialize(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset);
};

UCLASS()
class GP_A5_LITTLEPIPOTANK_API UNetworkProtocol : public UObject
{
	GENERATED_BODY()
};
