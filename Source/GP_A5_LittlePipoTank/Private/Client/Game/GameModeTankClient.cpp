// Fill out your copyright notice in the Description page of Project Settings.


#include "Client/Game/GameModeTankClient.h"

#include "GP_A5_LittlePipoTank.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Shared/LittlePipoTankGameInstance.h"
#include "Shared/NetworkProtocol.h"
#include "Shared/NetworkProtocolHelpers.h"
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

	// Send player inputs every ticks
	PredictClient(DeltaSeconds);

	// Interpolation
	InterpolateGame(DeltaSeconds);
}

void AGameModeTankClient::InitGameClient()
{
	InitializeNetwork(GetServerAdressIp());
	
	GameStateClient.ServerPeer = ServerPeer;

	ULittlePipoTankGameInstance* GameInstance = Cast<ULittlePipoTankGameInstance>(GetGameInstance());

	FPlayerNamePacket Packet = {.Name = GameInstance->GetPlayerName()};
	UNetworkProtocolHelpers::SendPacket(ServerPeer, Packet, ENET_PACKET_FLAG_RELIABLE);
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
		.PlayerInputs = PlayerInputs
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

			// Reconciliation
			ReconciliateClient(Packet.OwnPlayerData);
			
			GameStateClient.PlayersStateSnapshots.Add({.PlayerStates = Packet.OtherPlayersStateData});
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

		// Do the interpolation here
		for (const auto& FromPlayerData : FromSnapshot.PlayerStates)
		{

			// check if To snapshot has a position for the same player
			const FGameStatePacket::PlayerStateData* ToPlayerData = ToSnapshot.PlayerStates.FindByPredicate([&](const FGameStatePacket::PlayerStateData& PlayerData)
			{
				return PlayerData.Index == FromPlayerData.Index;
			});

			if (!ToPlayerData) continue;
			
			// Do Lerp
			FVector2D LerpLocation = FMath::Lerp(
				FromPlayerData.Location,
				ToPlayerData->Location,
				GameStateClient.SnapshotBufferAccumulator);
			
			float LerpRotation = FMath::Lerp(
				FromPlayerData.Rotation,
				ToPlayerData->Rotation,
				GameStateClient.SnapshotBufferAccumulator);
			
			float LerpAimRotation = FMath::Lerp(
				FromPlayerData.AimRotation,
				ToPlayerData->AimRotation,
				GameStateClient.SnapshotBufferAccumulator);

			// Apply Lerp Values
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
	// Update Pawn physics here ?
	
	SendClientPrediction();
}

void AGameModeTankClient::SendClientPrediction()
{
	// TODO Get Client Pawn And Possess it on receive player tank, so we don't cast every frame
	// TODO Maybe better: Input struct in player controller directly so easy to get

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	if (!PlayerController)
	{
		UE_LOGFMT(LogGP_A5_LittlePipoTank, Error, "Can't send inputs: Player controller is null.");
		return;
	}
	
	ATankPawn* PlayerTank = Cast<ATankPawn>(PlayerController->GetPawn());

	if (!PlayerTank)
	{
		UE_LOGFMT(LogGP_A5_LittlePipoTank, Error, "Can't send inputs: Player Tank is null.");
		return;
	}
	
	FPlayerInputsPacket InputsPacket = {
		.PlayerInputs = PlayerTank->GetTankInputs(),
		.PredictionIndex = GameStateClient.NextPredictionIndex
	};

	GameStateClient.Predictions.Add({
		.PredictionIndex = GameStateClient.NextPredictionIndex,
		.Inputs = PlayerTank->GetTankInputs(),
		//TODO Set Position, Rotation, AimRotation, Velocity
	});

	UNetworkProtocolHelpers::SendPacket(ServerPeer, InputsPacket, ENET_PACKET_FLAG_RELIABLE);

	GameStateClient.NextPredictionIndex++;
}

void AGameModeTankClient::ReconciliateClient(const FGameStatePacket::OwnPlayerStateData& OwnPlayerData)
{
	// TODO Prep Reconciliation but need Predicted Inputs back in OwnPlayerData
	//while (!GameStateClient.Predictions.IsEmpty() && GameStateClient.Predictions[0].PredictionIndex < OwnPlayerData)
}
