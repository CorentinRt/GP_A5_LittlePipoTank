// Fill out your copyright notice in the Description page of Project Settings.


#include "Server/Game/GameModeTankServer.h"

#include "Server/Game/PlayerTankSpawnPoint.h"
#include "Shared/LittlePipoTankGameInstance.h"
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
	
	ULittlePipoTankGameInstance* GameInstance = Cast<ULittlePipoTankGameInstance>(GetGameInstance());
	InitializeNetwork(GameInstance->GetAppPort());

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

	UpdateTanksPhysics(DeltaTime);
	UpdateBulletsPhysics(DeltaTime);
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
			GameStatePacket.OwnPlayerData.PlayerTankInputs = LocalPlayer.LastPlayerInputs.PlayerInputs;
			GameStatePacket.OwnPlayerData.PredictionIndex = LocalPlayer.LastPlayerInputs.PredictionIndex;
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
				OtherPlayerData.AimRotation = OtherPlayer.PlayerTanks->GetHeadAimRotation();
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

				NewBulletStateData.Location = LocalBullet->GetActorLocation();
				
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

	for (FPlayerDataServer& LocalPlayer : GameStateServer.Players)
	{
		if (LocalPlayer.Peer == nullptr)
			continue;

		// Game Phase
		FGamePhasePacket GamePhasePacket = {};
		GamePhasePacket.GamePhase = GameStateServer.CurrentGamePhase;
		UNetworkProtocolHelpers::SendPacket(LocalPlayer.Peer, GamePhasePacket, ENET_PACKET_FLAG_RELIABLE);
	}
}

void AGameModeTankServer::ReactChangeGamePhase(ETankGamePhase InGamePhase)
{
	Super::ReactChangeGamePhase(InGamePhase);

	switch (InGamePhase)
	{
	case ETankGamePhase::WAITING_PLAYER:
	case ETankGamePhase::PRE_GAME:
		{
			for (FPlayerDataServer& LocalPlayer : GameStateServer.Players)
			{
				if (LocalPlayer.Peer == nullptr)
					continue;

				SpawnTankPlayer(LocalPlayer);
			}
			
			SendTankSpawnToAllClients();
			break;
		}
	case ETankGamePhase::IN_GAME:
		break;
	case ETankGamePhase::POST_GAME:
		{
			SaveVictoriousPlayer();
			NotifyVictoriousPlayer();
			break;
		}
	case ETankGamePhase::NONE:
		break;
	default:
		break;
	}

	TanksReactToOnChangeGamePhase(InGamePhase);
	BulletsReactToOnChangeGamePhase(InGamePhase);

	if (GameStateServer.CurrentGamePhase == ETankGamePhase::PRE_GAME)
	{
		DestroyAllBullets();
	}
}

void AGameModeTankServer::TanksReactToOnChangeGamePhase(ETankGamePhase InGamePhase)
{
	for (FPlayerDataServer& LocalPlayer : GameStateServer.Players)
	{
		if (LocalPlayer.Peer == nullptr)
			continue;

		if (!IsValid(LocalPlayer.PlayerTanks))
			continue;

		LocalPlayer.PlayerTanks->ReactOnGamePhaseChanged_Implementation(InGamePhase);
	}
}

void AGameModeTankServer::BulletsReactToOnChangeGamePhase(ETankGamePhase InGamePhase)
{
	for (int i = 0; i < GameStateServer.AllTankBullets.Num(); ++i)
	{
		ATankBullet* LocalBullet = GameStateServer.AllTankBullets[i];
		
		if (!IsValid(LocalBullet))
			continue;

		LocalBullet->ReactOnGamePhaseChanged_Implementation(InGamePhase);
	}
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
	case ETankGamePhase::IN_GAME:
		{
			if (GetTanksAliveCount() <= 1)
			{
				CurrentAccumulatedGamePhaseTime = 0.f;
				NextGamePhase();
				break;
			}
			
			while (CurrentAccumulatedGamePhaseTime >= GetGamePhaseDuration(GameStateServer.CurrentGamePhase))
			{
				CurrentAccumulatedGamePhaseTime -= GetGamePhaseDuration(GameStateServer.CurrentGamePhase);
				NextGamePhase();
			}

			CurrentAccumulatedGamePhaseTime += DeltaTime;
			
			break;
		}
	case ETankGamePhase::PRE_GAME:
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

void AGameModeTankServer::UpdateTanksPhysics(float DeltaTime)
{
	for (int i = 0; i < GameStateServer.Players.Num(); ++i)
	{
		FPlayerDataServer& LocalPlayer = GameStateServer.Players[i];

		if (LocalPlayer.Peer == nullptr)
			continue;

		float Advancement = 1.f;

		if (LocalPlayer.InputBuffer.Num() < TargetInputBufferSize)
		{
			Advancement -= (TargetInputBufferSize - LocalPlayer.InputBuffer.Num()) * 0.05f;
		}
		else if (LocalPlayer.InputBuffer.Num() > TargetInputBufferSize)
		{
			Advancement += (LocalPlayer.InputBuffer.Num() - TargetInputBufferSize) * 0.05f;
		}

		LocalPlayer.InputBufferAdvancement += Advancement;

		if (LocalPlayer.InputBufferAdvancement >= 1.f)
		{
			LocalPlayer.LastPlayerInputs = {};

			while (LocalPlayer.InputBufferAdvancement >= 1.f)
			{
				if (!LocalPlayer.InputBuffer.IsEmpty())
				{
					FPlayerInputsPacket ExtractedInput = LocalPlayer.InputBuffer[0];
					LocalPlayer.LastPlayerInputs.PlayerInputs.MoveInput = ExtractedInput.PlayerInputs.MoveInput;
					LocalPlayer.LastPlayerInputs.PlayerInputs.AimInput = ExtractedInput.PlayerInputs.AimInput;
					LocalPlayer.LastPlayerInputs.PlayerInputs.FireInput |= ExtractedInput.PlayerInputs.FireInput;
					LocalPlayer.LastPlayerInputs.PredictionIndex = ExtractedInput.PredictionIndex;
					LocalPlayer.InputBuffer.RemoveAt(0);
				}
				
				LocalPlayer.InputBufferAdvancement -= 1.f;
			}
		}

		if (!IsValid(LocalPlayer.PlayerTanks))
			continue;

		LocalPlayer.PlayerTanks->SetPlayerTankInputs(LocalPlayer.LastPlayerInputs.PlayerInputs);
		LocalPlayer.PlayerTanks->UpdatePhysics(DeltaTime);
	}
}

void AGameModeTankServer::UpdateBulletsPhysics(float DeltaTime)
{
	for (int i = 0; i < GameStateServer.AllTankBullets.Num(); ++i)
	{
		ATankBullet* Bullet = GameStateServer.AllTankBullets[i];

		if (!IsValid(Bullet))
			continue;

		Bullet->UpdatePhysics(DeltaTime);
	}

	for (int i = 0; i < GameStateServer.AllTankBullets.Num(); ++i)
	{
		ATankBullet* Bullet = GameStateServer.AllTankBullets[i];

		if (!IsValid(Bullet))
			continue;

		if (Bullet->CheckBulletDestroyed())
		{
			--i;
		}
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
					LocalPlayer.InputBuffer.Add(PlayerInputsPacket);
					
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
	if (InPlayer.PlayerIndex >= PlayersSpawnPoints.Num() || InPlayer.PlayerIndex < 0)
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

		InPlayer.PlayerTanks->OnSpawnBullet.AddDynamic(this, &AGameModeTankServer::BindTankSpawnBullet);
		InPlayer.PlayerTanks->OnTankDestroyed.AddDynamic(this, &AGameModeTankServer::BindHandleTankDestroyed);
	}
	
	InPlayer.PlayerTanks->SetActorHiddenInGame(false);
	InPlayer.PlayerTanks->SetActorEnableCollision(true);
	InPlayer.PlayerTanks->SetActorLocation(SpawnPoint->GetActorLocation());
	InPlayer.PlayerTanks->SetActorRotation(SpawnPoint->GetActorRotation());
	
	return true;
}

int AGameModeTankServer::GetTanksAliveCount() const
{
	int Count = 0;
	
	for (int i = 0; i < GameStateServer.Players.Num(); ++i)
	{
		if (i >= PlayersSpawnPoints.Num())
			continue;
		
		const FPlayerDataServer& LocalPlayer = GameStateServer.Players[i];

		if (LocalPlayer.Peer == nullptr)
			continue;

		if (!IsValid(LocalPlayer.PlayerTanks))
			continue;

		if (LocalPlayer.PlayerTanks->IsHidden())
			continue;

		++Count;
	}

	return Count;
}

void AGameModeTankServer::SaveVictoriousPlayer()
{
	for (int i = 0; i < GameStateServer.Players.Num(); ++i)
	{
		if (i >= PlayersSpawnPoints.Num())
			continue;
		
		const FPlayerDataServer& LocalPlayer = GameStateServer.Players[i];

		if (LocalPlayer.Peer == nullptr)
			continue;

		if (!IsValid(LocalPlayer.PlayerTanks))
			continue;

		if (LocalPlayer.PlayerTanks->IsHidden())
			continue;

		LastVictoriousPlayerIndex = LocalPlayer.PlayerIndex;
		return;
	}
}

void AGameModeTankServer::NotifyVictoriousPlayer()
{
	OnPlayerVictory.Broadcast(GetLastVictoriousPlayerData());
}

void AGameModeTankServer::DestroyAllBullets()
{
	for (int i = 0; i < GameStateServer.AllTankBullets.Num(); ++i)
	{
		ATankBullet* Bullet = GameStateServer.AllTankBullets[i];

		if (IsValid(Bullet))
		{
			GetWorld()->DestroyActor(Bullet);
		}
	}

	GameStateServer.AllTankBullets.Empty();
}

FPlayerDataServer AGameModeTankServer::GetLastVictoriousPlayerData()
{
	if (LastVictoriousPlayerIndex < 0)
		return {};

	
	for (int i = 0; i < GameStateServer.Players.Num(); ++i)
	{
		if (i >= PlayersSpawnPoints.Num())
			continue;
		
		const FPlayerDataServer& LocalPlayer = GameStateServer.Players[i];

		if (LocalPlayer.Peer == nullptr)
			continue;

		if (LocalPlayer.PlayerIndex != LastVictoriousPlayerIndex)
			continue;

		return LocalPlayer;
	}

	return {};
}

void AGameModeTankServer::PlayerJoined(ENetPeer* InPeer, const FString& InPlayerName)
{
	++GameStateServer.PlayerCount;
	
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
	NewPlayerData.LastPlayerInputs.PlayerInputs = PlayerInputs;
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

	if (GameStateServer.CurrentGamePhase == ETankGamePhase::WAITING_PLAYER)
	{
		if (SpawnTankPlayer(NewPlayerData))
		{
			
		}
		
		// Send Tank Spawned
		SendTankSpawnToAllClients();
		
		// Game Phase
		FGamePhasePacket GamePhasePacket = {};
		GamePhasePacket.GamePhase = GameStateServer.CurrentGamePhase;
		UNetworkProtocolHelpers::SendPacket(NewPlayerData.Peer, GamePhasePacket, ENET_PACKET_FLAG_RELIABLE);
	}

	UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Player count = {0}", GameStateServer.PlayerCount);
}

void AGameModeTankServer::PlayerLeft(const ENetEvent& event, int IndexToRemove)
{
	--GameStateServer.PlayerCount;
	
	GameStateServer.Players[IndexToRemove].Peer = nullptr;
	
	if (GameStateServer.Players[IndexToRemove].PlayerTanks)
	{
		GetWorld()->DestroyActor(GameStateServer.Players[IndexToRemove].PlayerTanks);
		GameStateServer.Players[IndexToRemove].PlayerTanks = nullptr;
	}

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

	if (GameStateServer.PlayerCount < 2)
	{
		if (GameStateServer.CurrentGamePhase != ETankGamePhase::WAITING_PLAYER)
		{
			SetServerGamePhase(ETankGamePhase::WAITING_PLAYER);
		}
	}
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
			TankSpawnData.SpawnLocation = FVector(ListedPlayer.PlayerTanks->GetActorLocation());
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
