// Fill out your copyright notice in the Description page of Project Settings.


#include "Client/Game/GameModeTankClient.h"

#include "GP_A5_LittlePipoTank.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Client/ClientPlayerController.h"
#include "Client/Game/ClientTankBullet.h"
#include "Client/Game/ClientTankPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Internal/Kismet/BlueprintTypeConversions.h"
#include "Server/Game/PlayerTankSpawnPoint.h"
#include "Shared/LittlePipoTankGameInstance.h"
#include "Shared/NetworkProtocol.h"
#include "Shared/NetworkProtocolHelpers.h"
#include "Shared/TankCustomization/PlayersTankColorData.h"
#include "TankClasses/TankPawn.h"

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

	// Interpolation
	InterpolateGame(DeltaSeconds);
}

void AGameModeTankClient::InitGameClient()
{
	GetAllPlayerSpawnPoints();
	
	InitializeNetwork(GetServerAdressIp());
	
	GameStateClient.ServerPeer = ServerPeer;

	ULittlePipoTankGameInstance* GameInstance = Cast<ULittlePipoTankGameInstance>(GetGameInstance());

	FPlayerNamePacket Packet = {.Name = GameInstance->GetPlayerName()};
	UNetworkProtocolHelpers::SendPacket(ServerPeer, Packet, ENET_PACKET_FLAG_RELIABLE);

	PlayerController = Cast<AClientPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	if (!PlayerController)
	{
		UE_LOGFMT(LogGP_A5_LittlePipoTank, Error, "Failed to get Client Player Controller");
	}
}

void AGameModeTankClient::GamePhysicsTick(float DeltaTime)
{
	Super::GamePhysicsTick(DeltaTime);

	// Send player inputs every ticks
	PredictClient(DeltaTime);
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
	
	for (const FPlayerDataClient& LocalPlayer : GameStateClient.Players)
	{
		if (LocalPlayer.PlayerIndex == PlayerIndex)
		{
			PlayerPeerAlreadyExists = true;
			break;
		}
	}

	if (PlayerPeerAlreadyExists)
		return;
	
	FPlayerTankInputs PlayerInputs
	{
		.MoveInput = FVector2D::ZeroVector,
		.AimInput = FVector2D::ZeroVector,
		.FireInput = false
	};
	
	FPlayerDataClient NewPlayerData
	{
		.PlayerIndex = PlayerIndex,
		.PlayerName = PlayerName,
	};

	
}

void AGameModeTankClient::ReceivePlayerLeaveGame()
{
	int PlayerIndex = 0;
	FString PlayerName = "NULL_NAME";
	ENetPeer* Peer = nullptr;

	for (int i = 0; i < GameStateClient.Players.Num(); ++i)
	{
		FPlayerDataClient& LocalPlayer = GameStateClient.Players[i];

		if (LocalPlayer.PlayerIndex == PlayerIndex)
		{
			GameStateClient.Players.RemoveAt(i);
			return;
		}
	}
}

FString AGameModeTankClient::GetServerAdressIp() const
{
	ULittlePipoTankGameInstance* TankGameInstance = Cast<ULittlePipoTankGameInstance>(UGameplayStatics::GetGameInstance(this));
	if(!TankGameInstance)
		return "localhost";
	
	return TankGameInstance->GetServerIp();
}

void AGameModeTankClient::HandleMessage(const OpCode& OpCode, const TArray<BYTE>& ByteArray,
                                        TArray<BYTE>::SizeType& Offset, ENetPeer* Peer)
{
	Super::HandleMessage(OpCode, ByteArray, Offset, Peer);

	switch (OpCode)
	{
	case OpCode::S_GameState:
		{
			FGameStatePacket Packet = {};
			Packet.Deserialize(ByteArray, Offset);

			// UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Received Game State");
			
			// Reconciliation
			ReconciliateClient(Packet.OwnPlayerData);
			
			GameStateClient.PlayersStateSnapshots.Add({
				.OtherPlayerStates = Packet.OtherPlayersStateData,
				.OwnPlayerState = Packet.OwnPlayerData,
				.BulletsStates = Packet.BulletsStateData
			});

			// Remove bullets not here anymore
			for (auto It = GameStateClient.Bullets.CreateIterator(); It;)
			{
				FGameStatePacket::BulletStateData* Bullet = Packet.BulletsStateData.FindByPredicate([&](const FGameStatePacket::BulletStateData& BulletData)
				{
					return BulletData.Index == It->BulletIndex;
				});

				if (!Bullet)
				{
					if (It->Bullet)
					{
						GetWorld()->DestroyActor(It->Bullet);
						It->Bullet = nullptr;
					}
					
					It.RemoveCurrent();
					continue;
				}

				++It;
			}
			UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "S_PlayerList: Nb players after removal: {0}", GameStateClient.Players.Num());

			
			// Add bullets not present
			for (auto It = Packet.BulletsStateData.CreateIterator(); It; ++It)
			{
				// UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "PlayerList Add Loop: Player Index = {0}", It->Index);
				
				const FBulletDataClient* Bullet =  GameStateClient.Bullets.FindByPredicate([&](const FBulletDataClient& BulletData)
				{
					return BulletData.BulletIndex == It->Index;
				});

				if (!Bullet)
				{
					// UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "And ClientTank");
					FVector SpawnLocation(It->Location.X, It->Location.Y, 0.0f);
					FRotator SpawnRotation(0.0f, It->Rotation, 0.0f);
					
					FActorSpawnParameters SpawnParameters;
					SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					AClientTankBullet* BulletActor = GetWorld()->SpawnActor<AClientTankBullet>(
						BlueprintClientBulletClass,
						SpawnLocation,
						SpawnRotation,
						SpawnParameters);

					if (BulletActor == nullptr)
					{
						UE_LOGFMT(LogGP_A5_LittlePipoTank, Error, "Failed to cast spawned bullet to ClientBullet class");
						continue;
					}
					
					GameStateClient.Bullets.Add({
						.BulletIndex = It->Index,
						.Bullet = BulletActor,
					});
				}
			}
			
			break;
		}

	case OpCode::S_InitClientData:
		{
			FInitClientDataPacket Packet = {};
			Packet.Deserialize(ByteArray, Offset);

			UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Received InitClientData with OwnPlayerIndex: {0}", Packet.OwnPlayerIndex);
			GameStateClient.OwnPlayerIndex = Packet.OwnPlayerIndex;
			break;
		}

	case OpCode::S_PlayerList:
		{
			FPlayerListPacket Packet = {};
			Packet.Deserialize(ByteArray, Offset);

			UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Received S_PlayerList:");
			
			// Remove players not here anymore
			for (auto It = GameStateClient.Players.CreateIterator(); It;)
			{
				FPlayerListPacket::Player* Player = Packet.Players.FindByPredicate([&](const FPlayerListPacket::Player& PlayerData)
				{
					return PlayerData.Index == It->PlayerIndex;
				});

				if (!Player)
				{
					if (It->Tank)
					{
						GetWorld()->DestroyActor(It->Tank);
						It->Tank = nullptr;
					}
					
					It.RemoveCurrent();
					continue;
				}

				++It;
			}
			UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "S_PlayerList: Nb players after removal: {0}", GameStateClient.Players.Num());

			
			// Add players not present
			for (auto It = Packet.Players.CreateIterator(); It; ++It)
			{
				// UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "PlayerList Add Loop: Player Index = {0}", It->Index);
				
				const FPlayerDataClient* Player =  GameStateClient.Players.FindByPredicate([&](const FPlayerDataClient& PlayerData)
				{
					return PlayerData.PlayerIndex == It->Index;
				});

				if (!Player)
				{
					GameStateClient.Players.Add({
						.PlayerIndex = It->Index,
						.PlayerName = It->Name,
						.Tank = nullptr
					});
				}
			}
			UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "S_PlayerList: Nb players after add: {0}", GameStateClient.Players.Num());
			
			break;
		}

	case OpCode::S_GamePhase:
		{
			FGamePhasePacket Packet = {};
			Packet.Deserialize(ByteArray, Offset);

			SetClientGamePhase(Packet.GamePhase);

			switch (GameStateClient.CurrentGamePhase)
			{
			case ETankGamePhase::NONE:
				{
					break;
				}
			case ETankGamePhase::WAITING_PLAYER:
				{
					break;
				}
			case ETankGamePhase::PRE_GAME:
				{
					break;
				}
			case ETankGamePhase::IN_GAME:
				{
					break;
				}
			case ETankGamePhase::POST_GAME:
				{
					break;
				}
			}
			
			break;
		}

	case OpCode::S_SpawnTank:
		{
			FSpawnTankPacket Packet = {};
			Packet.Deserialize(ByteArray, Offset);
			UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Receive Spawn ClientTank");

			for (const FSpawnTankPacket::TankSpawnData& SpawnTankData : Packet.TankSpawnsData)
			{
				FPlayerDataClient* Player =  GameStateClient.Players.FindByPredicate([&](const FPlayerDataClient& PlayerData)
				{
					return PlayerData.PlayerIndex == SpawnTankData.PlayerIndex;
				});

				if (!Player) continue;
				if (Player->Tank) continue;
			
				UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Try Spawn ClientTank");

				FVector SpawnLocation(SpawnTankData.SpawnLocation.X, SpawnTankData.SpawnLocation.Y, 0.0f);
				FRotator SpawnRotation(0.0f, SpawnTankData.SpawnRotation, 0.0f);
				
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				AClientTankPawn* PlayerTank = GetWorld()->SpawnActor<AClientTankPawn>(
					BlueprintClientTankClass,
					SpawnLocation,
					SpawnRotation,
					SpawnParameters);
			
				if (PlayerTank == nullptr)
				{
					UE_LOGFMT(LogGP_A5_LittlePipoTank, Error, "Failed to cast spawned tank to ClientTank class");
					continue;
				}

				Player->Tank = PlayerTank;

				Player->Tank->ReactOnGamePhaseChanged_Implementation(GameStateClient.CurrentGamePhase);

				UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Spawn ClientTank Success");

				if (!PlayersColorData) continue;
				
				if (Player->PlayerIndex >= PlayersColorData->TankColors.Num())
				{
					Player->Tank->SetMeshMaterial(PlayersColorData->DefaultColor);
					
				}
				else
				{
					Player->Tank->SetMeshMaterial(PlayersColorData->TankColors[Player->PlayerIndex]);
				}
			}
			
			break;
		}

	case OpCode::S_DestroyTank:
		{
			FDestroyTankPacket Packet = {};
			Packet.Deserialize(ByteArray, Offset);
			UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Receive Destroy ClientTank");

			FPlayerDataClient* Player =  GameStateClient.Players.FindByPredicate([&](const FPlayerDataClient& PlayerData)
			{
				return PlayerData.PlayerIndex == Packet.PlayerIndex;
			});

			if (!Player || !Player->Tank) break;
			UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Try Destroy ClientTank");

			Player->Tank->ReceiveOnTankDestroy();
			GetWorld()->DestroyActor(Player->Tank);
			Player->Tank = nullptr;
			UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Destroy ClientTank Success");
			
			break;
		}
	}
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

void AGameModeTankClient::InterpolateGame(float DeltaTime)
{
	// Snapshot Jitter Buffer Management
	if (GameStateClient.PlayersStateSnapshots.Num() >= 2)
	{
		// Compute increment
		float Increment = (DeltaTime / TickDelayNetwork);

		// Compute playback rate
		float PlaybackRate = 1.0f;
		
		int SnapshotBufferSize = GameStateClient.PlayersStateSnapshots.Num();
		if (SnapshotBufferSize > GameStateClient.SnapshotBufferTargetSize)
			PlaybackRate += 0.05f * (SnapshotBufferSize - GameStateClient.SnapshotBufferTargetSize);
		else if (SnapshotBufferSize > GameStateClient.SnapshotBufferTargetSize)
			PlaybackRate -= 0.05f * (GameStateClient.SnapshotBufferTargetSize - SnapshotBufferSize);

		// Add to accumulator
		GameStateClient.SnapshotBufferAccumulator +=  Increment * PlaybackRate; // Hard code '10' To be replace by network tick delay

		const FInterpolationSnapshot& FromSnapshot = GameStateClient.PlayersStateSnapshots[0];
		const FInterpolationSnapshot& ToSnapshot = GameStateClient.PlayersStateSnapshots[1];

		// Temp for test: Own Player Interp
		// TODO Remove Client Player Interpsd
		
		// {
		// 	// check if To snapshot has a position for the same player
		// 	FPlayerDataClient* OwnPlayerData = GameStateClient.Players.FindByPredicate([&](const FPlayerDataClient& PlayerData)
		// 	{
		// 		return PlayerData.PlayerIndex == GameStateClient.OwnPlayerIndex;
		// 	});
		//
		// 	if (!OwnPlayerData)
		// 	{
		// 		UE_LOGFMT(LogGP_A5_LittlePipoTank, Error, "No Own Data");
		// 	}
		//
		// 	if (!OwnPlayerData->Tank)
		// 	{
		// 		UE_LOGFMT(LogGP_A5_LittlePipoTank, Error, "No Own Tank");
		// 	}
		// 	
		// 	if (OwnPlayerData && OwnPlayerData->Tank)
		// 	{
		// 		FGameStatePacket::OwnPlayerStateData FromPlayerData = FromSnapshot.OwnPlayerState;
		// 		FGameStatePacket::OwnPlayerStateData ToPlayerData = ToSnapshot.OwnPlayerState;
		// 		
		// 		// Do Lerp
		// 		FVector2D LerpLocation = FMath::Lerp(
		// 			FromPlayerData.Location,
		// 			ToPlayerData.Location,
		// 			GameStateClient.SnapshotBufferAccumulator);
		//
		// 		FRotator LerpRotation = UKismetMathLibrary::RLerp(
		// 			FRotator(0.0f, FromPlayerData.Rotation, 0.0f),
		// 			FRotator(0.0f, ToPlayerData.Rotation, 0.0f),
		// 			GameStateClient.SnapshotBufferAccumulator,
		// 			true);
		// 		
		// 		FRotator LerpAimRotation = UKismetMathLibrary::RLerp(
		// 			FRotator(0.0f, FromPlayerData.AimRotation, 0.0f),
		// 			FRotator(0.0f, ToPlayerData.AimRotation, 0.0f),
		//
		// 			GameStateClient.SnapshotBufferAccumulator,
		// 			true);
		//
		// 		//Apply Own Player Lerp
		// 		OwnPlayerData->Tank->SetLocation(LerpLocation, false);
		// 		OwnPlayerData->Tank->SetRotation(LerpRotation);
		// 		OwnPlayerData->Tank->SetAimRotation(LerpAimRotation);
		//
		// 		// UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Lerp Own Tank");
		// 	}
		// }
		
		// Do the interpolation here
		for (const auto& FromPlayerData : FromSnapshot.OtherPlayerStates)
		{
			// check if To snapshot has a position for the same player
			const FGameStatePacket::PlayerStateData* ToPlayerData = ToSnapshot.OtherPlayerStates.FindByPredicate([&](const FGameStatePacket::PlayerStateData& PlayerData)
			{
				return PlayerData.Index == FromPlayerData.Index;
			});

			if (!ToPlayerData) continue;

			// check if player is in game data
			FPlayerDataClient* OtherPlayerData = GameStateClient.Players.FindByPredicate([&](const FPlayerDataClient& PlayerData)
			{
				return PlayerData.PlayerIndex == FromPlayerData.Index;
			});

			if (!OtherPlayerData || !OtherPlayerData->Tank) continue;
			
			// Do Lerp
			FVector2D LerpLocation = FMath::Lerp(
				FromPlayerData.Location,
				ToPlayerData->Location,
				GameStateClient.SnapshotBufferAccumulator);

			FRotator LerpRotation = UKismetMathLibrary::RLerp(
					FRotator(0.0f, FromPlayerData.Rotation, 0.0f),
					FRotator(0.0f, ToPlayerData->Rotation, 0.0f),
					GameStateClient.SnapshotBufferAccumulator,
					true);
			
			FRotator LerpAimRotation = UKismetMathLibrary::RLerp(
					FRotator(0.0f, FromPlayerData.AimRotation, 0.0f),
					FRotator(0.0f, ToPlayerData->AimRotation, 0.0f),

					GameStateClient.SnapshotBufferAccumulator,
					true);

			// Apply Lerp Values
			OtherPlayerData->Tank->SetLocation(LerpLocation, false);
			OtherPlayerData->Tank->SetRotation(LerpRotation);
			OtherPlayerData->Tank->SetAimRotation(LerpAimRotation);
		}

		// Interp Bullets
		for (const auto& FromBulletData : FromSnapshot.BulletsStates)
		{
			// check if To snapshot has a position for the same player
			const FGameStatePacket::BulletStateData* ToBulletData = ToSnapshot.BulletsStates.FindByPredicate([&](const FGameStatePacket::BulletStateData& BulletData)
			{
				return BulletData.Index == FromBulletData.Index;
			});

			if (!ToBulletData) continue;

			// check if bullet is in game data
			FBulletDataClient* Bullet = GameStateClient.Bullets.FindByPredicate([&](const FBulletDataClient& BulletData)
			{
				return BulletData.BulletIndex == FromBulletData.Index;
			});

			if (!Bullet || !Bullet->Bullet) continue;
			
			// Do Lerp
			FVector2D LerpLocation = FMath::Lerp(
				FromBulletData.Location,
				ToBulletData->Location,
				GameStateClient.SnapshotBufferAccumulator);

			FRotator LerpRotation = UKismetMathLibrary::RLerp(
					FRotator(0.0f, FromBulletData.Rotation, 0.0f),
					FRotator(0.0f, ToBulletData->Rotation, 0.0f),
					GameStateClient.SnapshotBufferAccumulator,
					true);

			// Apply Lerp Values
			Bullet->Bullet->SetLocation(LerpLocation, false);
			Bullet->Bullet->SetRotation(LerpRotation);
		}
		
		// Look if we need to 'reset' accumulator
		if (GameStateClient.SnapshotBufferAccumulator < 1.0f) return;
		
		GameStateClient.SnapshotBufferAccumulator -= 1.0f;
		GameStateClient.PlayersStateSnapshots.RemoveAt(0);
	}
	else if (GameStateClient.PlayersStateSnapshots.Num() == 1)
	{
		//TODO We set the game state to be the same as the only snapshot
	}
	else
	{
		// Do Nothing, we can't interpolate.
	}
}


void AGameModeTankClient::PredictClient(float DeltaTime)
{
	// TODO Update Pawn physics here ?
	// TODO Block Inputs if not in IN_GAME


	FPlayerDataClient* PlayerData = GameStateClient.Players.FindByPredicate([&](const FPlayerDataClient& Player)
	{
		return Player.PlayerIndex == GameStateClient.OwnPlayerIndex;
	});

	if (!PlayerData || !PlayerData->Tank) return;
	
	const FPlayerTankInputs& ConsumedInput = PlayerController->GetTankInputs();
	
	PlayerData->Tank->SetPlayerTankInputs(ConsumedInput);
	PlayerData->Tank->UpdatePhysics(DeltaTime);

	
	GameStateClient.Predictions.Add({
		.PredictionIndex = GameStateClient.NextPredictionIndex,
		.Inputs = ConsumedInput,
		//TODO Set Position, Rotation, AimRotation, Velocity
	});
	
	SendClientPrediction();
}

void AGameModeTankClient::SendClientPrediction()
{
	if (!PlayerController)
	{
		UE_LOGFMT(LogGP_A5_LittlePipoTank, Error, "Can't send inputs: Player controller is null.");
		return;
	}

	FPlayerTankInputs& CurrentInputs = PlayerController->GetTankInputs();
	
	FPlayerInputsPacket InputsPacket = {
		.PlayerInputs = CurrentInputs,
		.PredictionIndex = GameStateClient.NextPredictionIndex
	};

	UNetworkProtocolHelpers::SendPacket(ServerPeer, InputsPacket, ENET_PACKET_FLAG_RELIABLE);

	GameStateClient.NextPredictionIndex++;

	if (CurrentInputs.FireInput) CurrentInputs.FireInput = false;
}

void AGameModeTankClient::ReconciliateClient(const FGameStatePacket::OwnPlayerStateData& OwnPlayerData)
{
	// TODO Prep Reconciliation but need Predicted Inputs back in OwnPlayerData
	//while (!GameStateClient.Predictions.IsEmpty() && GameStateClient.Predictions[0].PredictionIndex < OwnPlayerData)
}
