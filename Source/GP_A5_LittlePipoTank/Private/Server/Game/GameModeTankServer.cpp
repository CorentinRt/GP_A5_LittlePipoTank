// Fill out your copyright notice in the Description page of Project Settings.


#include "Server/Game/GameModeTankServer.h"

#include "Server/Game/PlayerTankSpawnPoint.h"
#include "Shared/NetworkProtocol.h"
#include "Shared/NetworkProtocolHelpers.h"
#include "TankClasses/TankBullet.h"
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
	Super::GamePhysicsTick(DeltaTime);
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
	Super::GameNetworkTick(DeltaTime);
	
	SendGameStatePacketToAllClients();
}

void AGameModeTankServer::SendGameStatePacketToAllClients()
{
	for (FPlayerDataServer& LocalPlayer : GameStateServer.Players)
	{
		if (LocalPlayer.Peer == nullptr)
			continue;
		
		FGameStatePacket GameStatePacket = {};

		GameStatePacket.OwnPlayerData.Index = LocalPlayer.PlayerIndex;

		// Own Player
		if (LocalPlayer.PlayerTanks)
		{
			GameStatePacket.OwnPlayerData.Location = FVector2D(LocalPlayer.PlayerTanks->GetActorLocation().X, LocalPlayer.PlayerTanks->GetActorLocation().Y);
			GameStatePacket.OwnPlayerData.Rotation = LocalPlayer.PlayerTanks->GetActorRotation().Yaw;
			GameStatePacket.OwnPlayerData.AimRotation = LocalPlayer.PlayerTanks->GetHeadAimRotation();
			GameStatePacket.OwnPlayerData.Velocity = FVector2D(LocalPlayer.PlayerTanks->GetVelocity().X, LocalPlayer.PlayerTanks->GetVelocity().Y);
		}

		// Other Players
		for (FPlayerDataServer& OtherPlayer : GameStateServer.Players)
		{
			if (OtherPlayer.Peer == nullptr)
				continue;
			
			if (OtherPlayer.PlayerIndex == LocalPlayer.PlayerIndex)
				continue;

			FGameStatePacket::PlayerStateData OtherPlayerData = {};

			OtherPlayerData.Index = OtherPlayer.PlayerIndex;
			
			if (OtherPlayer.PlayerTanks)
			{
				OtherPlayerData.Location = FVector2D(OtherPlayer.PlayerTanks->GetActorLocation().X, OtherPlayer.PlayerTanks->GetActorLocation().Y);
				OtherPlayerData.Rotation = OtherPlayer.PlayerTanks->GetActorRotation().Yaw;
				OtherPlayerData.AimRotation = OtherPlayer.PlayerTanks->GetBaseAimRotation().Yaw;
			}

			GameStatePacket.OtherPlayersStateData.Add(MoveTemp(OtherPlayerData));
		}

		// Bullets
		for (int i = 0; i < GameStateServer.AllTankBullets.Num(); ++i)
		{
			ATankBullet* LocalBullet = GameStateServer.AllTankBullets[i];
			
			if (!IsValid(LocalBullet))
			{
				GameStateServer.AllTankBullets.RemoveAt(i);
				--i;
				continue;
			}
			else
			{
				FGameStatePacket::BulletStateData NewBulletStateData = {};

				NewBulletStateData.Index = LocalBullet->BulletIndex;

				FVector BulletLocation = LocalBullet->GetActorLocation();
				NewBulletStateData.Location = FVector2D(BulletLocation.X, BulletLocation.Y);
				
				NewBulletStateData.Rotation = LocalBullet->GetActorRotation().Yaw;

				GameStatePacket.BulletsStateData.Add(MoveTemp(NewBulletStateData));
			}
		}
		
		UNetworkProtocolHelpers::SendPacket(LocalPlayer.Peer, GameStatePacket, ENET_PACKET_FLAG_RELIABLE);
	}
}

ETankGamePhase AGameModeTankServer::GetCurrentGamePhase()
{
	return GameStateServer.CurrentGamePhase;
}

void AGameModeTankServer::SetServerGamePhase(ETankGamePhase NewGamePhase)
{
	SetGamePhase(GameStateServer.CurrentGamePhase, NewGamePhase);
}

void AGameModeTankServer::ReactChangeGamePhase(ETankGamePhase InGamePhase)
{
	Super::ReactChangeGamePhase(InGamePhase);

	for (FPlayerDataServer& LocalPlayer : GameStateServer.Players)
	{
		if (LocalPlayer.Peer == nullptr)
			continue;

		SpawnTankPlayer(LocalPlayer);
	}
	
	SendTankSpawnToAllClients();
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
				if (LocalPlayer.Peer == nullptr)
					continue;
				
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
	case OpCode::C_PlayerInputs:
		{
			FPlayerInputsPacket PlayerInputsPacket = {};

			PlayerInputsPacket.Deserialize(ByteArray, Offset);

			for (FPlayerDataServer& LocalPlayer : GameStateServer.Players)
			{
				if (LocalPlayer.Peer == nullptr)
					continue;
				
				if (LocalPlayer.Peer == Peer)
				{
					LocalPlayer.PlayerInputs = PlayerInputsPacket.PlayerInputs;

					if (LocalPlayer.PlayerTanks)
						LocalPlayer.PlayerTanks->SetPlayerTankInputs(LocalPlayer.PlayerInputs);
					break;
				}
			}
			
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

		if (LocalPlayer.Peer == nullptr)
			continue;
		
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

bool AGameModeTankServer::SpawnTankPlayer(FPlayerDataServer& InPlayer)
{
	if (InPlayer.PlayerIndex >= PlayersSpawnPoints.Num())
		return false;

	APlayerTankSpawnPoint* SpawnPoint = PlayersSpawnPoints[InPlayer.PlayerIndex];

	if (!SpawnPoint)
		return false;
	
	if (!InPlayer.PlayerTanks)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		InPlayer.PlayerTanks = GetWorld()->SpawnActor<ATankPawn>(TankPawnClassBlueprint, SpawnParameters);
		InPlayer.PlayerTanks->ReactOnGamePhaseChanged_Implementation(GameStateServer.CurrentGamePhase);

		UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Bind Tank Spawn Bullet");
		InPlayer.PlayerTanks->OnSpawnBullet.AddDynamic(this, &AGameModeTankServer::BindTankSpawnBullet);
		InPlayer.PlayerTanks->OnTankDestroyed.AddDynamic(this, &AGameModeTankServer::BindHandleTankDestroyed);
	}
	
	InPlayer.PlayerTanks->SetActorHiddenInGame(false);
	InPlayer.PlayerTanks->SetActorEnableCollision(true);
	InPlayer.PlayerTanks->SetActorLocation(SpawnPoint->GetActorLocation());
	InPlayer.PlayerTanks->SetActorRotation(SpawnPoint->GetActorRotation());
	
	return true;
}

void AGameModeTankServer::PlayerJoined(ENetPeer* InPeer, const FString& InPlayerName)
{
	++GameStateServer.PlayerCount;

	if (GameStateServer.PlayerCount > 2)
		return;
	
	FPlayerTankInputs PlayerInputs
	{
		.MoveInput = FVector2D::ZeroVector,
		.AimInput = FVector2D::ZeroVector,
		.FireInput = false
	};

	FPlayerDataServer& NewPlayerData = GetAvailableNewPlayerDataOrCreate();

	if (NewPlayerData.PlayerIndex < 0)
	{
		NewPlayerData.PlayerIndex = GameStateServer.NextPlayerIndex++;
	}

	NewPlayerData.PlayerName = InPlayerName;
	NewPlayerData.PlayerInputs = PlayerInputs;
	NewPlayerData.Peer = InPeer;

	GEngine->AddOnScreenDebugMessage(
			-1,
			10.f,
			FColor::Yellow,
			TEXT("Success Player created !")
			);

	// Client Init
	FInitClientDataPacket NewPlayerInitPacket
	{
		.OwnPlayerIndex = NewPlayerData.PlayerIndex
	};
	UNetworkProtocolHelpers::SendPacket(NewPlayerData.Peer, NewPlayerInitPacket, ENET_PACKET_FLAG_RELIABLE);

	// Player List
	for (FPlayerDataServer& LocalPlayer : GameStateServer.Players)
	{
		if (LocalPlayer.Peer == nullptr)
			continue;
		
		FPlayerListPacket PlayerListPacket;

		for (FPlayerDataServer& ListedPlayer : GameStateServer.Players)
		{
			if (ListedPlayer.Peer == nullptr)
				continue;
			
			FPlayerListPacket::Player PlayerListSingle
			{
				.Name = ListedPlayer.PlayerName,
				.Index = ListedPlayer.PlayerIndex
			};
			
			PlayerListPacket.Players.Add(MoveTemp(PlayerListSingle));
		}
		UNetworkProtocolHelpers::SendPacket(LocalPlayer.Peer, PlayerListPacket, ENET_PACKET_FLAG_RELIABLE);
	}

	if (SpawnTankPlayer(NewPlayerData))
	{
		
	}
	
	// Send Tank Spawned
	SendTankSpawnToAllClients();
}

void AGameModeTankServer::PlayerLeft(const ENetEvent& event, int IndexToRemove)
{
	--GameStateServer.PlayerCount;
	
	GameStateServer.Players[IndexToRemove].Peer = nullptr;
	GetWorld()->DestroyActor(GameStateServer.Players[IndexToRemove].PlayerTanks);

	for (FPlayerDataServer& LocalPlayer : GameStateServer.Players)
	{
		if (LocalPlayer.Peer == nullptr)
			continue;
		
		FPlayerListPacket PlayerListPacket;
		
		for (FPlayerDataServer& ListedPlayer : GameStateServer.Players)
		{
			if (ListedPlayer.Peer == nullptr)
				continue;
			
			FPlayerListPacket::Player PlayerListSingle
			{
				.Name = ListedPlayer.PlayerName,
				.Index = LocalPlayer.PlayerIndex
			};
			
			PlayerListPacket.Players.Add(MoveTemp(PlayerListSingle));
		}

		UNetworkProtocolHelpers::SendPacket(LocalPlayer.Peer, PlayerListPacket, ENET_PACKET_FLAG_RELIABLE);
	}

	SetServerGamePhase(ETankGamePhase::WAITING_PLAYER);
}

void AGameModeTankServer::BindTankSpawnBullet(ATankBullet* InTankBullet)
{
	if (!InTankBullet)
		return;

	
	InTankBullet->BulletIndex = GameStateServer.NextBulletIndex++;

	InTankBullet->OnBulletDestroyed.AddDynamic(this, &AGameModeTankServer::BindHandleBulletDestroyed);
	
	GameStateServer.AllTankBullets.Add(InTankBullet);
	UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Add bullet to list count = {0}", GameStateServer.AllTankBullets.Num());
}

void AGameModeTankServer::BindHandleBulletDestroyed(ATankBullet* InTankBullet)
{
	if (!InTankBullet)
		return;
	
	InTankBullet->OnBulletDestroyed.RemoveDynamic(this, &AGameModeTankServer::BindHandleBulletDestroyed);

	if (GameStateServer.AllTankBullets.Contains(InTankBullet))
	{
		GameStateServer.AllTankBullets.Remove(InTankBullet);
		UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Remove bullet from list count = {0}", GameStateServer.AllTankBullets.Num());
	}
}

void AGameModeTankServer::BindHandleTankDestroyed(ATankPawn* InTank)
{
	int AssociatedPlayerIndex = -1;
	for (FPlayerDataServer& CheckPlayerTank : GameStateServer.Players)
	{
		if (CheckPlayerTank.PlayerTanks == InTank)
		{
			AssociatedPlayerIndex = CheckPlayerTank.PlayerIndex;
			break;
		}
	}

	if (AssociatedPlayerIndex < 0)
		return;
	
	FDestroyTankPacket DestroyedTankPacket = {};
	DestroyedTankPacket.PlayerIndex = AssociatedPlayerIndex;
	
	for (FPlayerDataServer& LocalPlayer : GameStateServer.Players)
	{
		UNetworkProtocolHelpers::SendPacket(LocalPlayer.Peer, DestroyedTankPacket, ENET_PACKET_FLAG_RELIABLE);
	}
}

void AGameModeTankServer::SendTankSpawnToAllClients()
{
	for (FPlayerDataServer& LocalPlayer : GameStateServer.Players)
	{
		if (LocalPlayer.Peer == nullptr)
			continue;
		
		FSpawnTankPacket SpawnTankPacket = {};

		for (FPlayerDataServer& ListedPlayer : GameStateServer.Players)
		{
			if (ListedPlayer.Peer == nullptr || ListedPlayer.PlayerTanks == nullptr)
				continue;

			FSpawnTankPacket::TankSpawnData TankSpawnData = {};

			TankSpawnData.PlayerIndex = ListedPlayer.PlayerIndex;
			TankSpawnData.SpawnLocation = FVector2D(ListedPlayer.PlayerTanks->GetActorLocation());
			TankSpawnData.SpawnRotation = ListedPlayer.PlayerTanks->GetActorRotation().Yaw;
			
			SpawnTankPacket.TankSpawnsData.Add(MoveTemp(TankSpawnData));
		}
	
		UNetworkProtocolHelpers::SendPacket(LocalPlayer.Peer, SpawnTankPacket, ENET_PACKET_FLAG_RELIABLE);
	}
}

FPlayerDataServer& AGameModeTankServer::GetAvailableNewPlayerDataOrCreate()
{
	for (FPlayerDataServer& LocalPlayer : GameStateServer.Players)
	{
		if (LocalPlayer.Peer == nullptr)
		{
			return LocalPlayer;
		}
	}

	FPlayerDataServer NewPlayerData = {};
	
	GameStateServer.Players.Add(MoveTemp(NewPlayerData));

	return GameStateServer.Players[GameStateServer.Players.Num() - 1];
}
