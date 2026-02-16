// Fill out your copyright notice in the Description page of Project Settings.


#include "Server/Game/GameModeTankServer.h"

void AGameModeTankServer::BeginPlay()
{
	Super::BeginPlay();

	InitGameServer();

	SetGamePhase(ETankGamePhase::WAITING_PLAYER);
}

void AGameModeTankServer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateCheckTickPhysics(DeltaSeconds);
	UpdateCheckTickNetwork(DeltaSeconds);
}

void AGameModeTankServer::UpdateCheckTickPhysics(float DeltaTime)
{
	CurrentAccumulatedPhysicsTickTime += DeltaTime;

	while (CurrentAccumulatedPhysicsTickTime >= TickDelayPhysics)
	{
		GamePhysicsTick(TickDelayPhysics);
		CurrentAccumulatedPhysicsTickTime -= TickDelayPhysics;
	}
}

void AGameModeTankServer::UpdateCheckTickNetwork(float DeltaTime)
{
	CurrentAccumulatedNetworkTickTime += DeltaTime;

	while (CurrentAccumulatedNetworkTickTime >= TickDelayNetwork)
	{
		GameNetworkTick(TickDelayNetwork);
		CurrentAccumulatedNetworkTickTime -= TickDelayNetwork;
	}
}

void AGameModeTankServer::InitGameServer()
{


	IsServerInitialized = true;
}

void AGameModeTankServer::GamePhysicsTick(float DeltaTime)
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f,
		FColor::Yellow,
		TEXT("Tick Physics")
	);
}

void AGameModeTankServer::GameNetworkTick(float DeltaTime)
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f,
		FColor::Blue,
		TEXT("Tick Network")
	);
}

void AGameModeTankServer::SetGamePhase(ETankGamePhase GamePhase)
{
	CurrentTankGamePhase = GamePhase;
	ReactChangeGamePhase(CurrentTankGamePhase);
}

void AGameModeTankServer::NextGamePhase()
{
	switch (CurrentTankGamePhase)
	{
	case ETankGamePhase::WAITING_PLAYER:
		SetGamePhase(ETankGamePhase::PRE_GAME);
		CurrentTankGamePhase = ETankGamePhase::PRE_GAME;
		break;
	case ETankGamePhase::PRE_GAME:
		SetGamePhase(ETankGamePhase::IN_GAME);
		break;
	case ETankGamePhase::IN_GAME:
		SetGamePhase(ETankGamePhase::POST_GAME);
		break;
	case ETankGamePhase::POST_GAME:
		SetGamePhase(ETankGamePhase::PRE_GAME);
		break;
	default:
		break;
	}
}

void AGameModeTankServer::PlayerJoined()
{
	++PlayerCount;
	
}

void AGameModeTankServer::PlayerLeft()
{
	--PlayerCount;
	
}
