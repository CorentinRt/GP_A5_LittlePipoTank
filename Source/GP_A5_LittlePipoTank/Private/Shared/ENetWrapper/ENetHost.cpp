// Fill out your copyright notice in the Description page of Project Settings.
#include "Shared/ENetWrapper/ENetHost.h"

#include <string>

#include "GP_A5_LittlePipoTank.h"
#include "Elements/Framework/TypedElementQueryBuilder.h"
#include "Navigation/MetaNavMeshPath.h"
#include "Shared/ENetWrapper/ENetPeer.h"

bool FENetAddress::SetHost(const EENetAddressType& InType, const FString& InAddress)
{
	ENetAddressType Type = ENET_ADDRESS_TYPE_ANY;
	switch (InType)
	{
		case EENetAddressType::IpV4: Type = ENET_ADDRESS_TYPE_IPV4; break;
		case EENetAddressType::IpV6: Type = ENET_ADDRESS_TYPE_IPV6; break;
		case EENetAddressType::Any : Type = ENET_ADDRESS_TYPE_ANY ; break;
	}
	
	if (enet_address_set_host(&Address, Type, TCHAR_TO_UTF8(*InAddress)) != 0)
	{
		UE_LOGFMT(LogGP_A5_LittlePipoTank, Error, "Failed to resolve address");
		return false;
	}

	return true;
}

void FENetAddress::SetPort(UINT16 InPort)
{
	Address.port = InPort;
}

EENetAddressType FENetAddress::GetAddressType() const
{
	switch (Address.type)
	{
	case ENET_ADDRESS_TYPE_IPV4: return EENetAddressType::IpV4;
	case ENET_ADDRESS_TYPE_IPV6: return EENetAddressType::IpV6;
	case ENET_ADDRESS_TYPE_ANY : return EENetAddressType::Any;
	default: return EENetAddressType::Any;
	}
}

ENetAddress& FENetAddress::GetAddressHandle()
{
	return Address;
}

const ENetAddress& FENetAddress::GetAddressHandle() const
{
	return Address;
}


void UENetHost::BeginDestroy()
{
	UObject::BeginDestroy();

	if (Host)
	{
		enet_host_flush(Host);
		enet_host_destroy(Host);
	}
}

void UENetHost::Create(const EENetAddressType& InType, const FENetAddress* InAddress, SIZE_T AmountOfPeers)
{
	ENetAddressType Type = ENET_ADDRESS_TYPE_ANY;
	switch (InType)
	{
	case EENetAddressType::IpV4: Type = ENET_ADDRESS_TYPE_IPV4; break;
	case EENetAddressType::IpV6: Type = ENET_ADDRESS_TYPE_IPV6; break;
	case EENetAddressType::Any : Type = ENET_ADDRESS_TYPE_ANY ; break;
	}
	
	const ENetAddress* AddrPtr = nullptr;
	if (InAddress != nullptr)
	{
		AddrPtr = &InAddress->GetAddressHandle();
	}
	
	Host = enet_host_create(Type, AddrPtr, AmountOfPeers, 0, 0, 0);

	if (!Host)
		UE_LOGFMT(LogGP_A5_LittlePipoTank, Error, "Failed to create enet host");
}

void UENetHost::Connect(const FENetAddress& InAddress, UENetPeer& InPeer)
{
	InPeer.Init(enet_host_connect(Host, &InAddress.GetAddressHandle(), 0, 0));
}

bool UENetHost::Service(UINT32 timeToWait) const
{
	ENetEvent Event = {};
	return enet_host_service(Host, &Event, timeToWait) > 0;	
}

void UENetHost::ServiceAndCheckEvents() const
{
	ENetEvent event;
	if (enet_host_service(Host, &event, 1) > 0)
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
		while (enet_host_check_events(Host, &event) > 0);	
	}
}

const ENetHost* UENetHost::GetHandle() const
{
	return Host;
}
