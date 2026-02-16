// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/ENetWrapper/ENetGameInstanceSubsystem.h"
#include "GP_A5_LittlePipoTank.h"
#include "ENet6/enet.h"

void UENetGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (enet_initialize() < 0)
		UE_LOGFMT(LogGP_A5_LittlePipoTank, Fatal, "Failed to initialize enet");
}

void UENetGameInstanceSubsystem::Deinitialize()
{
	Super::Deinitialize();

	enet_deinitialize();
}
