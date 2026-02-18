// Fill out your copyright notice in the Description page of Project Settings.


#include "Client/Game/GameModeTankClient.h"

#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Shared/LittlePipoTankGameInstance.h"
#include "Shared/NetworkProtocol.h"
#include "Shared/NetworkProtocolHelpers.h"

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
	InitializeNetwork(GetServerAdressIp());
	
	GameStateClient.ServerPeer = ServerPeer;

	ULittlePipoTankGameInstance* GameInstance = Cast<ULittlePipoTankGameInstance>(GetGameInstance());

	FPlayerNamePacket Packet = {.Name = GameInstance->GetPlayerName()};
	UNetworkProtocolHelpers::SendPacket(*ServerPeer, Packet, ENET_PACKET_FLAG_RELIABLE);
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
