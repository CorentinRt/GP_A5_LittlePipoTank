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

	SetGamePhase(GameStateServer.CurrentGamePhase, ETankGamePhase::WAITING_PLAYER);
}

void AGameModeTankServer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void AGameModeTankServer::InitGameServer()
{


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

void AGameModeTankServer::NextGamePhase()
{
	switch (GameStateServer.CurrentGamePhase)
	{
	case ETankGamePhase::WAITING_PLAYER:
		SetGamePhase(GameStateServer.CurrentGamePhase, ETankGamePhase::PRE_GAME);
		break;
	case ETankGamePhase::PRE_GAME:
		SetGamePhase(GameStateServer.CurrentGamePhase, ETankGamePhase::IN_GAME);
		break;
	case ETankGamePhase::IN_GAME:
		SetGamePhase(GameStateServer.CurrentGamePhase, ETankGamePhase::POST_GAME);
		break;
	case ETankGamePhase::POST_GAME:
		SetGamePhase(GameStateServer.CurrentGamePhase, ETankGamePhase::PRE_GAME);
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

void AGameModeTankServer::RegisterListener(AActor* InGamePhaseListener)
{
	if (!InGamePhaseListener || !InGamePhaseListener->GetClass()->ImplementsInterface(UGamePhaseListener::StaticClass()))
		return;

	GamePhaseListeners.AddUnique(InGamePhaseListener);
}

void AGameModeTankServer::UnregisterListener(AActor* InGamePhaseListener)
{
	if (!InGamePhaseListener || !InGamePhaseListener->GetClass()->ImplementsInterface(UGamePhaseListener::StaticClass()))
		return;

	GamePhaseListeners.Remove(InGamePhaseListener);
}

void AGameModeTankServer::PlayerJoined()
{
	++GameStateServer.PlayerCount;


	FPlayerData NewPlayerData
	{
		.PlayerIndex = GameStateServer.NextPlayerIndex++,
		.PlayerName = "NULL_NAME"
	};

	GameStateServer.Players.Add(NewPlayerData);
}

void AGameModeTankServer::PlayerLeft()
{
	--GameStateServer.PlayerCount;

	// Remove player on leave using its connexion peer enet
}
