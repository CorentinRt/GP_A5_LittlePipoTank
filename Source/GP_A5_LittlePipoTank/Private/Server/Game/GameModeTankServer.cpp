// Fill out your copyright notice in the Description page of Project Settings.


#include "Server/Game/GameModeTankServer.h"

#include "Shared/Game/GamePhaseListener.h"

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

	
}

void AGameModeTankServer::HandleConnection(const ENetEvent& event)
{
	Super::HandleConnection(event);

	if (event.peer == nullptr)
		return;

	bool PlayerPeerAlreadyExists = false;
	
	for (const FPlayerData& LocalPlayer : GameStateServer.Players)
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
	
	for (const FPlayerData& LocalPlayer : GameStateServer.Players)
	{
		if (LocalPlayer.Peer == event.peer)
		{
			PlayerLeft(event, LocalPlayer);
			return;
		}
	}
}

void AGameModeTankServer::PlayerJoined(const ENetEvent& event)
{
	++GameStateServer.PlayerCount;

	FPlayerTankInputs PlayerInputs
	{
		.MovementsInputs = 0.f,
		.LookDirInputs = 0.f
	};

	FPlayerData NewPlayerData
	{
		.PlayerIndex = GameStateServer.NextPlayerIndex++,
		.PlayerName = "NULL_NAME",
		.PlayerInputs = PlayerInputs,
		.Peer = event.peer
	};

	GameStateServer.Players.Add(NewPlayerData);
}

void AGameModeTankServer::PlayerLeft(const ENetEvent& event, const FPlayerData& PlayerLeave)
{
	--GameStateServer.PlayerCount;

	GameStateServer.Players.Remove(PlayerLeave);
}
