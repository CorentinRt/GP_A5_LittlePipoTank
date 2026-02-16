// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/ENetWrapper/ENetGameMode.h"

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
	// Empty
}
