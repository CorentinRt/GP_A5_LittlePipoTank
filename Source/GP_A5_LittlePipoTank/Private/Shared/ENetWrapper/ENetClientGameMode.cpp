// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/ENetWrapper/ENetClientGameMode.h"

#include "GP_A5_LittlePipoTank.h"
#include "Shared/NetworkProtocol.h"
#include "Shared/NetworkProtocolHelpers.h"

void AENetClientGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	RunNetwork();
}

void AENetClientGameMode::InitializeNetwork()
{
	Super::InitializeNetwork();

	// Unwrapped test
	{
		ENetAddress serverAddress;
		if (enet_address_set_host(&serverAddress, ENET_ADDRESS_TYPE_ANY, "localhost") != 0)
		{
			UE_LOGFMT(LogGP_A5_LittlePipoTank, Error, "Failed to resolve address");
			return;
		}
		serverAddress.port = 10001;

		Host = enet_host_create(serverAddress.type, nullptr, 1, 0, 0, 0);
		if (!Host)
		{
			UE_LOGFMT(LogGP_A5_LittlePipoTank, Error, "Failed to create ENet host");
			return;
		}
		

		ServerPeer = enet_host_connect(Host, &serverAddress, 0, 0);
		{
			for (std::size_t i = 0; i < 100; ++i)
			{
				ENetEvent event;
				if (enet_host_service(Host, &event, 100) > 0)
				{
					// Nous avons un événement, la connexion a soit pu s'effectuer (ENET_EVENT_TYPE_CONNECT) soit échoué (ENET_EVENT_TYPE_DISCONNECT)
					break; //< On sort de la boucle
				}
			}

			if (enet_peer_get_state(ServerPeer) != ENET_PEER_STATE_CONNECTED)
			{
				UE_LOGFMT(LogGP_A5_LittlePipoTank, Error, "Failed to connect");
				return;
			}

			// Si on arrive ici, on est connecté !
			UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Connected!");
		}
	}
}

void AENetClientGameMode::FinalizeNetwork()
{
	if (ServerPeer)
	{
		enet_peer_disconnect_now(ServerPeer, 0);
	}
	
	Super::FinalizeNetwork();
}

void AENetClientGameMode::RunNetwork()
{
	Super::RunNetwork();
	
	// Unwrap test
	// {
	// 	if (!ServerPeer) return;
	//
	// 	FExemplePacket Packet = {};
	// 	Packet.X = 32.32f;
	//
	// 	enet_peer_send(ServerPeer, 0, UNetworkProtocolHelpers::BuildENetPacket(Packet, ENET_PACKET_FLAG_RELIABLE));
	// }
}

void AENetClientGameMode::HandleMessage(const OpCode& OpCode, const TArray<BYTE>& ByteArray,
	TArray<BYTE>::SizeType& Offset)
{
	Super::HandleMessage(OpCode, ByteArray, Offset);

	switch (OpCode)
	{
	case OpCode::S_ExempleCode:
		{
			FExemplePacket Packet = {};
			Packet.Deserialize(ByteArray, Offset);

			UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Receive exemple packet with x value of: {0}", Packet.X);
		}
	}
}
