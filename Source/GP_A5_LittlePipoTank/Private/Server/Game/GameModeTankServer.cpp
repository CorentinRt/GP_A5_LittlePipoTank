// Fill out your copyright notice in the Description page of Project Settings.


#include "Server/Game/GameModeTankServer.h"

#include "Kismet/GameplayStatics.h"
#include "Shared/NetworkProtocol.h"
#include "Shared/NetworkProtocolHelpers.h"
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
		SetServerGamePhase(ETankGamePhase::WAITING_PLAYER);
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
		if (GameStateServer.PlayerCount >= 2)
		{
			while (CurrentAccumulatedGamePhaseTime >= GetGamePhaseDuration(GameStateServer.CurrentGamePhase))
			{
				CurrentAccumulatedGamePhaseTime -= GetGamePhaseDuration(GameStateServer.CurrentGamePhase);
				NextGamePhase();
			}

			CurrentAccumulatedGamePhaseTime += DeltaTime;
		}
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
		{
			Duration = GamePhasesData->WaitingPlayerDuration;
		}
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
	TArray<BYTE>::SizeType& Offset, ENetPeer* Peer)
{
	Super::HandleMessage(OpCode, ByteArray, Offset, Peer);

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

			bool PlayerPeerAlreadyExists = false;
	
			for (const FPlayerDataServer& LocalPlayer : GameStateServer.Players)
			{
				if (LocalPlayer.Peer == Peer)
				{
					PlayerPeerAlreadyExists = true;
					break;
				}
			}

			if (PlayerPeerAlreadyExists)
				return;

			PlayerJoined(Peer, PlayerNamePacket.Name);
			
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
		TEXT("A client is trying to connect to server !")
		);
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
			if (LocalPlayer.PlayerTanks)
			{
				GetWorld()->DestroyActor(LocalPlayer.PlayerTanks);
			}
			
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

bool AGameModeTankServer::SpawnTankPlayer(FPlayerDataServer& InPlayer)
{
	if (NextSpawnPointIndex >= PlayersSpawnPoints.Num())
		return false;

	APlayerTankSpawnPoint* SpawnPoint = PlayersSpawnPoints[NextSpawnPointIndex++];

	if (!SpawnPoint)
		return false;
	
	if (!InPlayer.PlayerTanks)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		InPlayer.PlayerTanks = GetWorld()->SpawnActor<ATankPawn>(TankPawnClassBlueprint, SpawnParameters);
	}
	
	InPlayer.PlayerTanks->SetActorHiddenInGame(true);
	InPlayer.PlayerTanks->SetActorEnableCollision(true);
	InPlayer.PlayerTanks->SetActorLocation(SpawnPoint->GetActorLocation());
	InPlayer.PlayerTanks->SetActorRotation(SpawnPoint->GetActorRotation());

	return true;
}

void AGameModeTankServer::PlayerJoined(ENetPeer* InPeer, const FString& InPlayerName)
{
	++GameStateServer.PlayerCount;

	if (GameStateServer.PlayerCount >= 2)
		return;
	
	FPlayerTankInputs PlayerInputs
	{
		.MoveInput = FVector2D::ZeroVector,
		.AimInput = FVector2D::ZeroVector,
		.FireInput = false
	};

	FPlayerDataServer NewPlayerData
	{
		.PlayerIndex = GameStateServer.NextPlayerIndex++,
		.PlayerName = InPlayerName,
		.PlayerInputs = PlayerInputs,
		.Peer = InPeer
	};
	
	if (!SpawnTankPlayer(NewPlayerData))
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(
			-1,
			10.f,
			FColor::Yellow,
			TEXT("Success Player created !")
			);
	
	GameStateServer.Players.Add(MoveTemp(NewPlayerData));

	FInitClientDataPacket NewPlayerInitPacket
	{
		.OwnPlayerIndex = NewPlayerData.PlayerIndex
	};

	UNetworkProtocolHelpers::SendPacket(NewPlayerData.Peer, NewPlayerInitPacket, ENET_PACKET_FLAG_RELIABLE);
	
	for (FPlayerDataServer& LocalPlayer : GameStateServer.Players)
	{
		FPlayerListPacket PlayerListPacket;

		for (FPlayerDataServer& ListedPlayer : GameStateServer.Players)
		{
			FPlayerListPacket::Player PlayerListSingle
			{
				.Name = ListedPlayer.PlayerName,
				.Index = LocalPlayer.PlayerIndex
			};
			
			PlayerListPacket.Players.Add(MoveTemp(PlayerListSingle));
		}
		
		UNetworkProtocolHelpers::SendPacket(LocalPlayer.Peer, PlayerListPacket, ENET_PACKET_FLAG_RELIABLE);
	}
}

void AGameModeTankServer::PlayerLeft(const ENetEvent& event, int IndexToRemove)
{
	--GameStateServer.PlayerCount;
	
	GameStateServer.Players.RemoveAt(IndexToRemove);

	for (FPlayerDataServer& LocalPlayer : GameStateServer.Players)
	{
		// Build packet and send it

		//UNetworkProtocolHelpers::SendPacket(LocalPlayer.Peer, PlayerLeftPacket, ENET_PACKET_FLAG_RELIABLE);
	}

	SetServerGamePhase(ETankGamePhase::WAITING_PLAYER);
}
