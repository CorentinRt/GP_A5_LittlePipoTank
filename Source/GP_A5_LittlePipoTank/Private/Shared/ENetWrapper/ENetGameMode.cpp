// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/ENetWrapper/ENetGameMode.h"

#include "GP_A5_LittlePipoTank.h"

AENetGameMode::AENetGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AENetGameMode::BeginPlay()
{
	Super::BeginPlay();

	InitializeNetwork();
}

void AENetGameMode::BeginDestroy()
{
	Super::BeginDestroy();

	FinalizeNetwork();
}

void AENetGameMode::InitializeNetwork()
{
	// Empty
}

void AENetGameMode::FinalizeNetwork()
{
	if (Host)
	{
		enet_host_flush(Host);
		enet_host_destroy(Host);
	}
}

void AENetGameMode::RunNetwork()
{
	if (!Host) return;
		
	ENetEvent event;
	if (enet_host_service(Host, &event, 1) > 0)
	{
		do
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				{
					OnNetworkEventConnect(event);
					break;
				}

			case ENET_EVENT_TYPE_RECEIVE:
				{
					OnNetworkEventReceive(event);
					break;
				}

			case ENET_EVENT_TYPE_DISCONNECT:
				{
					OnNetworkEventDisconnect(event);
					break;
				}

			case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
				{
					OnNetworkEventDisconnectTimeout(event);
					break;
				}

			case ENET_EVENT_TYPE_NONE:
				{
					UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Unexpected event none");
					break;
				}
			}
		}
		while (enet_host_check_events(Host, &event) > 0);
	}
}

void AENetGameMode::OnNetworkEventConnect(const ENetEvent& event)
{
	UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Event connect");
}

void AENetGameMode::OnNetworkEventDisconnect(const ENetEvent& event)
{
	UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Event disconnect");
}

void AENetGameMode::OnNetworkEventDisconnectTimeout(const ENetEvent& event)
{
	UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Event timeout");
}

void AENetGameMode::OnNetworkEventReceive(const ENetEvent& event)
{
	UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Event receive");
}
