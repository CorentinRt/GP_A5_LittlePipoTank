// Fill out your copyright notice in the Description page of Project Settings.
#include "Shared/ENetWrapper/ENetHost.h"

#include "GP_A5_LittlePipoTank.h"
#include "Elements/Framework/TypedElementQueryBuilder.h"
#include "Navigation/MetaNavMeshPath.h"

void FENetAddress::SetHost(const EENetAddressType& InType, const FString& InAddress)
{
	ENetAddressType Type = ENET_ADDRESS_TYPE_ANY;
	switch (InType)
	{
		case EENetAddressType::IpV4: Type = ENET_ADDRESS_TYPE_IPV4; break;
		case EENetAddressType::IpV6: Type = ENET_ADDRESS_TYPE_IPV6; break;
		case EENetAddressType::Any : Type = ENET_ADDRESS_TYPE_ANY ; break;
	}
	
	if (!enet_address_set_host(&Address, Type, TCHAR_TO_UTF8(*InAddress)))
		UE_LOGFMT(LogGP_A5_LittlePipoTank, Error, "Failed to resolve address");

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
		AddrPtr = &InAddress->Address;
	}
	
	Host = enet_host_create(Type, AddrPtr, AmountOfPeers, 0, 0, 0);

	if (!Host)
		UE_LOGFMT(LogGP_A5_LittlePipoTank, Error, "Failed to create enet host");
}
