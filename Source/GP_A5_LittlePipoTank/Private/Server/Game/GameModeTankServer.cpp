// Fill out your copyright notice in the Description page of Project Settings.


#include "Server/Game/GameModeTankServer.h"

void AGameModeTankServer::BeginPlay()
{
	Super::BeginPlay();

	
}

void AGameModeTankServer::InitGameServer()
{


	IsServerInitialized = true;
}

void AGameModeTankServer::SetGamePhase(ETankGamePhase GamePhase)
{
	CurrentTankGamePhase = GamePhase;
	
}

void AGameModeTankServer::NextGamePhase()
{
	switch (CurrentTankGamePhase)
	{
	case ETankGamePhase::WAITING_PLAYER:
		CurrentTankGamePhase = ETankGamePhase::PRE_GAME;
		break;
	case ETankGamePhase::PRE_GAME:
		CurrentTankGamePhase = ETankGamePhase::IN_GAME;
		break;
	case ETankGamePhase::IN_GAME:
		CurrentTankGamePhase = ETankGamePhase::POST_GAME;
		break;
	case ETankGamePhase::POST_GAME:
		CurrentTankGamePhase = ETankGamePhase::PRE_GAME;
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
