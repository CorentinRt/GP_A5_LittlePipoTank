// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/ENetWrapper/ENetServerGameMode.h"

#include "GP_A5_LittlePipoTank.h"

void AENetServerGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	RunNetwork();
}

void AENetServerGameMode::InitializeNetwork()
{
	Super::InitializeNetwork();

	//Unwrapped test
	{
		ENetAddress address;
		enet_address_build_any(&address, ENET_ADDRESS_TYPE_IPV6);
		address.port = 10001;

		Host = enet_host_create(ENET_ADDRESS_TYPE_ANY, &address, 10, 0, 0, 0);
		if (!Host)
		{
			UE_LOGFMT(LogGP_A5_LittlePipoTank, Error, "Failed to create ENet host");
			return;
		}

		UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Successfully created ENet host");
	}
}


void AENetServerGameMode::OnNetworkEventConnect(const ENetEvent& event)
{
	Super::OnNetworkEventConnect(event);

	char address[128];
	enet_address_get_host(&event.peer->address, address, 128);

	UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "A new client (#{0}) connected from {1}!", event.peer->incomingPeerID, "address");
}

void AENetServerGameMode::OnNetworkEventDisconnect(const ENetEvent& event)
{
	Super::OnNetworkEventDisconnect(event);
}

void AENetServerGameMode::OnNetworkEventDisconnectTimeout(const ENetEvent& event)
{
	Super::OnNetworkEventDisconnectTimeout(event);
}

void AENetServerGameMode::OnNetworkEventReceive(const ENetEvent& event)
{
	Super::OnNetworkEventReceive(event);

	UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Received {0} from client (#{1})!", event.packet->dataLength, event.peer->incomingPeerID);
	UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Content: {0}", "content");
	// std::string(reinterpret_cast<const char*>(event.packet->data), event.packet->dataLength)
	enet_peer_send(event.peer, 0, event.packet);
}

