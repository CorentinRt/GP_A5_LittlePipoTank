// Fill out your copyright notice in the Description page of Project Settings.


#include "Client/Game/GameModeTankClient.h"

AGameModeTankClient::AGameModeTankClient()
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void AGameModeTankClient::BeginPlay()
{
	Super::BeginPlay();

	InitGameClient();
}

void AGameModeTankClient::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	
}

void AGameModeTankClient::InitGameClient()
{
	
}

void AGameModeTankClient::GamePhysicsTick(float DeltaTime)
{
	Super::GamePhysicsTick(DeltaTime);

	
}

void AGameModeTankClient::GameNetworkTick(float DeltaTime)
{
	Super::GameNetworkTick(DeltaTime);

	
}

ETankGamePhase AGameModeTankClient::GetCurrentGamePhase()
{
	return GameStateClient.CurrentGamePhase;
}

void AGameModeTankClient::SetClientGamePhase(ETankGamePhase NewGamePhase)
{
	SetGamePhase(GameStateClient.CurrentGamePhase, NewGamePhase);
}
