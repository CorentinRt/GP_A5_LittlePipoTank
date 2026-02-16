// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/ENetWrapper/ENetPeer.h"

#include "GP_A5_LittlePipoTank.h"
#include "enet6/enet.h"

void UENetPeer::Init(ENetPeer* InNetPeerHandle)
{
	Peer = InNetPeerHandle;
}

void UENetPeer::BeginDestroy()
{
	UObject::BeginDestroy();

	if (Peer)
	{
		enet_peer_disconnect_now(Peer, 0);
	}
}

bool UENetPeer::Send(ENetPacket* Packet) const
{
	if (!Peer) return false;
	if (enet_peer_send(Peer, 0, Packet) != 0)
	{
		UE_LOGFMT(LogGP_A5_LittlePipoTank, Error, "enet_peer_send_failed");
		return false;
	}
	
	return true;
}

bool UENetPeer::IsConnected() const
{
	if (!Peer) return false;
	return enet_peer_get_state(Peer) == ENET_PEER_STATE_CONNECTED;
}

const ENetPeer* UENetPeer::GetHandle() const
{
	return Peer;
}
