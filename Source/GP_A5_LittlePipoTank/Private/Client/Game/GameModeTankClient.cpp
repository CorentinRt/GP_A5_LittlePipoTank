// Fill out your copyright notice in the Description page of Project Settings.


#include "Client/Game/GameModeTankClient.h"

#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Kismet/GameplayStatics.h"

AGameModeTankClient::AGameModeTankClient()
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void AGameModeTankClient::BeginPlay()
{
	Super::BeginPlay();

	InitGameClient();
}

void AGameModeTankClient::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	RunNetwork();
}

void AGameModeTankClient::InitGameClient()
{
	InitializeNetwork();
	
	GameStateClient.ServerPeer = ServerPeer;
}

void AGameModeTankClient::GamePhysicsTick(float DeltaTime)
{
	Super::GamePhysicsTick(DeltaTime);

	
}

void AGameModeTankClient::GameNetworkTick(float DeltaTime)
{
	Super::GameNetworkTick(DeltaTime);
	
}

ETankGamePhase AGameModeTankClient::GetCurrentGamePhase()
{
	return GameStateClient.CurrentGamePhase;
}

void AGameModeTankClient::SetClientGamePhase(ETankGamePhase NewGamePhase)
{
	SetGamePhase(GameStateClient.CurrentGamePhase, NewGamePhase);
}

void AGameModeTankClient::ReceivePlayerJoinedGame()
{
	int PlayerIndex = 0;
	FString PlayerName = "NULL_NAME";
	ENetPeer* Peer = nullptr; 

	bool PlayerPeerAlreadyExists = false;
	
	for (const FPlayerData& LocalPlayer : GameStateClient.Players)
	{
		if (LocalPlayer.Peer == Peer)
		{
			PlayerPeerAlreadyExists = true;
			break;
		}
	}

	if (PlayerPeerAlreadyExists)
		return;
	
	FPlayerTankInputs PlayerInputs
	{
		.MovementsInputs = 0.f,
		.LookDirInputs = 0.f
	};
	
	FPlayerData NewPlayerData
	{
		.PlayerIndex = PlayerIndex,
		.PlayerName = PlayerName,
		.PlayerInputs = PlayerInputs,
		.Peer = Peer
	};

	
}

void AGameModeTankClient::ReceivePlayerLeaveGame()
{
	int PlayerIndex = 0;
	FString PlayerName = "NULL_NAME";
	ENetPeer* Peer = nullptr;

	for (int i = 0; i < GameStateClient.Players.Num(); ++i)
	{
		FPlayerData& LocalPlayer = GameStateClient.Players[i];

		if (LocalPlayer.Peer == Peer)
		{
			GameStateClient.Players.RemoveAt(i);
			return;
		}
	}
}

void AGameModeTankClient::HandleMessage(const OpCode& OpCode, const TArray<BYTE>& ByteArray,
                                        TArray<BYTE>::SizeType& Offset)
{
	Super::HandleMessage(OpCode, ByteArray, Offset);

	
}

void AGameModeTankClient::HandleConnection(const ENetEvent& event)
{
	Super::HandleConnection(event);

	// n'est pas censé arriver sur le client
}

void AGameModeTankClient::HandleDisconnection(const ENetEvent& event)
{
	Super::HandleDisconnection(event);
	
	// n'est pas censé arriver sur le client
}
