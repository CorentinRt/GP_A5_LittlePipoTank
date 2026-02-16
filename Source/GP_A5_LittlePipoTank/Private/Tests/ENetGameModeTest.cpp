// Fill out your copyright notice in the Description page of Project Settings.

#include "Tests/ENetGameModeTest.h"
#include "GP_A5_LittlePipoTank.h"
#include "Shared/ENetWrapper/ENetHost.h"

AENetGameModeTest::AENetGameModeTest()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AENetGameModeTest::BeginPlay()
{
	Super::BeginPlay();

	// FENetAddress ServerAddress = {};
	// ServerAddress.SetHost(EENetAddressType::Any, FString("localhost"));
	// ServerAddress.SetPort(10001);
	//
	// Host = NewObject<UENetHost>(this);
	// Host->Create(ServerAddress.GetAddressType(), &ServerAddress);

	ENetAddress serverAddress;
	if (enet_address_set_host(&serverAddress, ENET_ADDRESS_TYPE_ANY, "localhost") != 0)
	{
		UE_LOGFMT(LogGP_A5_LittlePipoTank, Error, "Failed to resolve address");
		return;
	}
	serverAddress.port = 10001;

	NetHost = enet_host_create(serverAddress.type, nullptr, 1, 0, 0, 0);
	if (!NetHost)
	{
		UE_LOGFMT(LogGP_A5_LittlePipoTank, Error, "Failed to create ENet host");
		return;
	}
		

	Peer = enet_host_connect(NetHost, &serverAddress, 0, 0);
	{
		for (std::size_t i = 0; i < 100; ++i)
		{
			ENetEvent event;
			if (enet_host_service(NetHost, &event, 100) > 0)
			{
				// Nous avons un événement, la connexion a soit pu s'effectuer (ENET_EVENT_TYPE_CONNECT) soit échoué (ENET_EVENT_TYPE_DISCONNECT)
				break; //< On sort de la boucle
			}
		}

		if (enet_peer_get_state(Peer) != ENET_PEER_STATE_CONNECTED)
		{
			UE_LOGFMT(LogGP_A5_LittlePipoTank, Error, "Failed to connect");
			return;
		}

		// Si on arrive ici, on est connecté !
		UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Connected!");
	}
}

void AENetGameModeTest::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "TICK!");
	if (!NetHost || !Peer) return;
	UE_LOGFMT(LogGP_A5_LittlePipoTank, Error, "TOCK!");

	FString message("Hello World!");
	ENetPacket* packet = enet_packet_create(TCHAR_TO_UTF8(*message), message.GetAllocatedSize(), ENET_PACKET_FLAG_RELIABLE);

	enet_peer_send(Peer, 0, packet);
	
	ENetEvent event;
	if (enet_host_service(NetHost, &event, 0) > 0)
	{
		do
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				{
					UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Unexpected connect event!");
					break;
				}

			case ENET_EVENT_TYPE_RECEIVE:
				{
					//std::cout << "Content: " << std::string(reinterpret_cast<const char*>(event.packet->data), event.packet->dataLength) << std::endl;
					UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Received {0} from client # {1}", event.packet->dataLength, event.peer->incomingPeerID);
					UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Content: ");

					enet_packet_destroy(event.packet);
					break;
				}

			case ENET_EVENT_TYPE_DISCONNECT:
				{
					UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Client #{0} disconnected", event.peer->incomingPeerID);
				
					break;
				}

			case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
				{
					UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Client #{0} timed out", event.peer->incomingPeerID);
					break;
				}

			case ENET_EVENT_TYPE_NONE:
				UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Unexpected event none");
				break;
			}
		}
		while (enet_host_check_events(NetHost, &event) > 0);	
	}
}
