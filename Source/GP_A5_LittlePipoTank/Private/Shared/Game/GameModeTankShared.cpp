// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/Game/GameModeTankShared.h"

#include "GP_A5_LittlePipoTank.h"
#include "Kismet/GameplayStatics.h"
#include "Server/Game/PlayerTankSpawnPoint.h"
#include "Shared/Game/GamePhaseListener.h"

AGameModeTankShared::AGameModeTankShared()
{
	PrimaryActorTick.bCanEverTick = true;

	bAsyncPhysicsTickEnabled = true;
}

void AGameModeTankShared::BeginPlay()
{
	Super::BeginPlay();

}

void AGameModeTankShared::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void AGameModeTankShared::AsyncPhysicsTickActor(float DeltaTime, float SimTime)
{
	Super::AsyncPhysicsTickActor(DeltaTime, SimTime);

	GEngine->AddOnScreenDebugMessage(
		-1,
		2.f,
		FColor::Red,
		FString::Printf(TEXT("Delta Time physics : %f"), DeltaTime)
	);
	
	UpdateCheckTickPhysics(DeltaTime);
	UpdateCheckTickNetwork(DeltaTime);
}

void AGameModeTankShared::UpdateCheckTickPhysics(float DeltaTime)
{
	CurrentAccumulatedPhysicsTickTime += DeltaTime;

	while (CurrentAccumulatedPhysicsTickTime >= TickDelayPhysics)
	{
		GamePhysicsTick(TickDelayPhysics);
		CurrentAccumulatedPhysicsTickTime -= TickDelayPhysics;
	}
}

void AGameModeTankShared::UpdateCheckTickNetwork(float DeltaTime)
{
	CurrentAccumulatedNetworkTickTime += DeltaTime;

	while (CurrentAccumulatedNetworkTickTime >= TickDelayNetwork)
	{
		GameNetworkTick(TickDelayNetwork);
		CurrentAccumulatedNetworkTickTime -= TickDelayNetwork;
	}
}

void AGameModeTankShared::GamePhysicsTick(float DeltaTime)
{
	
}

void AGameModeTankShared::GameNetworkTick(float DeltaTime)
{
	
}

ETankGamePhase AGameModeTankShared::GetCurrentGamePhase()
{
	return ETankGamePhase::NONE;
}

void AGameModeTankShared::SetGamePhase(ETankGamePhase& CurrentGamePhase, ETankGamePhase NewGamePhase)
{
	CurrentGamePhase = NewGamePhase;
	ReactChangeGamePhase(NewGamePhase);

	OnChangeGamePhase.Broadcast(NewGamePhase);
}

void AGameModeTankShared::GetAllPlayerSpawnPoints()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerTankSpawnPoint::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (APlayerTankSpawnPoint* SpawnPoint = Cast<APlayerTankSpawnPoint>(Actor))
			PlayersSpawnPoints.Add(SpawnPoint);
	}

	PlayersSpawnPoints.Sort([](const APlayerTankSpawnPoint& SpawnPointA, const APlayerTankSpawnPoint& SpawnPointB)
	{
		return SpawnPointA.IndexSpawn < SpawnPointB.IndexSpawn;
	});
	
	UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Amount of spawn points found: {0}", PlayersSpawnPoints.Num());
}

void AGameModeTankShared::ReactChangeGamePhase(ETankGamePhase InGamePhase)
{
	
}

