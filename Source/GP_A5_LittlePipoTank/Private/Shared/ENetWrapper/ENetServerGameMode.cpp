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

