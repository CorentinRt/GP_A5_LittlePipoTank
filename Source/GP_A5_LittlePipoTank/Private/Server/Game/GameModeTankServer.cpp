// Fill out your copyright notice in the Description page of Project Settings.


#include "Server/Game/GameModeTankServer.h"

#include "Kismet/GameplayStatics.h"
#include "Shared/NetworkProtocol.h"
#include "Shared/Game/GamePhaseListener.h"
#include "TankClasses/TankPawn.h"

AGameModeTankServer::AGameModeTankServer()
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void AGameModeTankServer::BeginPlay()
{
	Super::BeginPlay();

	InitGameServer();

	SetServerGamePhase(ETankGamePhase::WAITING_PLAYER);
}

void AGameModeTankServer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	RunNetwork();
}

void AGameModeTankServer::InitGameServer()
{
	GetAllPlayerSpawnPoints();
	
	InitializeNetwork();

	IsServerInitialized = true;
}

void AGameModeTankServer::GamePhysicsTick(float DeltaTime)
{
	/*
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f,
		FColor::Yellow,
		TEXT("Tick Physics")
	);
	*/

	UpdateCurrentGamePhase(DeltaTime);
}

void AGameModeTankServer::GameNetworkTick(float DeltaTime)
{
	/*
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f,
		FColor::Blue,
		TEXT("Tick Network")
	);
	*/
}

ETankGamePhase AGameModeTankServer::GetCurrentGamePhase()
{
	return GameStateServer.CurrentGamePhase;
}

void AGameModeTankServer::SetServerGamePhase(ETankGamePhase NewGamePhase)
{
	SetGamePhase(GameStateServer.CurrentGamePhase, NewGamePhase);
}

void AGameModeTankServer::NextGamePhase()
{
	switch (GameStateServer.CurrentGamePhase)
	{
	case ETankGamePhase::WAITING_PLAYER:
		SetServerGamePhase(ETankGamePhase::PRE_GAME);
		break;
	case ETankGamePhase::PRE_GAME:
		SetServerGamePhase(ETankGamePhase::IN_GAME);
		break;
	case ETankGamePhase::IN_GAME:
		SetServerGamePhase(ETankGamePhase::POST_GAME);
		break;
	case ETankGamePhase::POST_GAME:
		SetServerGamePhase(ETankGamePhase::PRE_GAME);
		break;
	default:
		break;
	}
}

void AGameModeTankServer::UpdateCurrentGamePhase(float DeltaTime)
{
	switch (GameStateServer.CurrentGamePhase)
	{
	case ETankGamePhase::WAITING_PLAYER:
		break;
	case ETankGamePhase::PRE_GAME:
	case ETankGamePhase::IN_GAME:
	case ETankGamePhase::POST_GAME:
		{
			while (CurrentAccumulatedGamePhaseTime >= GetGamePhaseDuration(GameStateServer.CurrentGamePhase))
			{
				CurrentAccumulatedGamePhaseTime -= GetGamePhaseDuration(GameStateServer.CurrentGamePhase);
				NextGamePhase();
			}

			CurrentAccumulatedGamePhaseTime += DeltaTime;
		}
		break;
	case ETankGamePhase::NONE:
		break;
	default:
		break;
	}
}

float AGameModeTankServer::GetGamePhaseDuration(ETankGamePhase InGamePhase)
{
	if (!GamePhasesData)
		return 0.f;

	float Duration = 0.f;
	
	switch (InGamePhase)
	{
	case ETankGamePhase::WAITING_PLAYER:
		break;
	case ETankGamePhase::PRE_GAME:
		{
			Duration = GamePhasesData->PreGameDuration;
		}
		break;
	case ETankGamePhase::IN_GAME:
		{
			Duration = GamePhasesData->InGameDuration;
		}
		break;
	case ETankGamePhase::POST_GAME:
		{
			Duration = GamePhasesData->PostGameDuration;
		}
		break;
	case ETankGamePhase::NONE:
		break;
	default:
		break;
	}

	return Duration;
}

void AGameModeTankServer::HandleMessage(const OpCode& OpCode, const TArray<BYTE>& ByteArray,
	TArray<BYTE>::SizeType& Offset)
{
	Super::HandleMessage(OpCode, ByteArray, Offset);

	switch (OpCode)
	{
	case OpCode::C_PlayerName:
		{
			FPlayerNamePacket PlayerNamePacket;

			PlayerNamePacket.Deserialize(ByteArray, Offset);
			
			GEngine->AddOnScreenDebugMessage(
				-1,
				10.f,
				FColor::Red,
				FString::Printf(TEXT("Receive msg name: %s"), *PlayerNamePacket.Name));
			break;
		}
	}
}

void AGameModeTankServer::HandleConnection(const ENetEvent& event)
{
	Super::HandleConnection(event);

	if (event.peer == nullptr)
		return;

	GEngine->AddOnScreenDebugMessage(
		-1,
		10.f,
		FColor::Yellow,
		TEXT("Client try connect to server !")
		);
	
	bool PlayerPeerAlreadyExists = false;
	
	for (const FPlayerDataServer& LocalPlayer : GameStateServer.Players)
	{
		if (LocalPlayer.Peer == event.peer)
		{
			PlayerPeerAlreadyExists = true;
			break;
		}
	}

	if (!PlayerPeerAlreadyExists)
		return;

	PlayerJoined(event);
}

void AGameModeTankServer::HandleDisconnection(const ENetEvent& event)
{
	Super::HandleDisconnection(event);

	if (event.peer == nullptr)
		return;

	for (int i = 0; i < GameStateServer.Players.Num(); ++i)
	{
		FPlayerDataServer& LocalPlayer = GameStateServer.Players[i];

		if (LocalPlayer.Peer == event.peer)
		{
			PlayerLeft(event, i);
			return;
		}
	}
}

void AGameModeTankServer::GetAllPlayerSpawnPoints()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerTankSpawnPoint::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		APlayerTankSpawnPoint* SpawnPoint = Cast<APlayerTankSpawnPoint>(Actor);

		if (SpawnPoint)
			PlayersSpawnPoints.Add(SpawnPoint);
	}
}

void AGameModeTankServer::SpawnTankPlayer(FPlayerDataServer& InPlayer, const APlayerTankSpawnPoint* InSpawnPoint)
{
	if (!InPlayer.PlayerTanks)
		return;

	InPlayer.PlayerTanks->SetActorHiddenInGame(true);
	InPlayer.PlayerTanks->SetActorLocation(InSpawnPoint->GetActorLocation());
	InPlayer.PlayerTanks->SetActorRotation(InSpawnPoint->GetActorRotation());
}

void AGameModeTankServer::PlayerJoined(const ENetEvent& event)
{
	++GameStateServer.PlayerCount;

	FPlayerTankInputs PlayerInputs
	{
		.MoveInput = FVector2D::ZeroVector,
		.AimInput = FVector2D::ZeroVector,
		.FireInput = false
	};

	FPlayerDataServer NewPlayerData
	{
		.PlayerIndex = GameStateServer.NextPlayerIndex++,
		.PlayerName = "NULL_NAME",
		.PlayerInputs = PlayerInputs,
		.Peer = event.peer
	};

	GEngine->AddOnScreenDebugMessage(
			-1,
			10.f,
			FColor::Yellow,
			TEXT("Success Player created !")
			);
	
	GameStateServer.Players.Add(MoveTemp(NewPlayerData));
}

void AGameModeTankServer::PlayerLeft(const ENetEvent& event, int IndexToRemove)
{
	--GameStateServer.PlayerCount;

	GameStateServer.Players.RemoveAt(IndexToRemove);
}
