// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/Game/GameModeTankShared.h"

#include "GP_A5_LittlePipoTank.h"
#include "Kismet/GameplayStatics.h"
#include "Server/Game/PlayerTankSpawnPoint.h"
#include "Shared/Game/GamePhaseListener.h"
#include "Shared/Game/PhysicsTickableShared.h"

AGameModeTankShared::AGameModeTankShared()
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void AGameModeTankShared::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsWithInterface(this, UPhysicsTickableShared::StaticClass(), PhysicsTickables);
}

void AGameModeTankShared::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateCheckTickPhysics(DeltaSeconds);
	UpdateCheckTickNetwork(DeltaSeconds);
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
	for (int i = 0; i < PhysicsTickables.Num(); ++i)
	{
		AActor* Tickable = PhysicsTickables[i];

		if (!IsValid(Tickable))
		{
			PhysicsTickables.RemoveAt(i);
			--i;
		}
	}
	
	for (AActor* Tickable : PhysicsTickables)
	{
		if (!IsValid(Tickable) || !Tickable->GetClass()->ImplementsInterface(UPhysicsTickableShared::StaticClass()))
			continue;

		IPhysicsTickableShared::Execute_OnTickPhysics_Blueprint(Tickable, DeltaTime);
	}
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
	for (int i = 0; i < GamePhaseListeners.Num(); ++i)
	{
		AActor* Listener = GamePhaseListeners[i];

		if (!IsValid(Listener))
		{
			GamePhaseListeners.RemoveAt(i);
			--i;
		}
	}
	
	for (AActor* GamePhaseListener : GamePhaseListeners)
	{
		if (!IsValid(GamePhaseListener) || !GamePhaseListener->GetClass()->ImplementsInterface(UGamePhaseListener::StaticClass()))
			continue;

		IGamePhaseListener::Execute_ReactOnGamePhaseChanged(GamePhaseListener, InGamePhase);
	}
	
	ReactChangeGamePhase_Implementation(InGamePhase);
}

void AGameModeTankShared::RegisterListener(AActor* InGamePhaseListener)
{
	if (!IsValid(InGamePhaseListener) || !InGamePhaseListener->GetClass()->ImplementsInterface(UGamePhaseListener::StaticClass()))
		return;

	GamePhaseListeners.AddUnique(InGamePhaseListener);
}

void AGameModeTankShared::UnregisterListener(AActor* InGamePhaseListener)
{
	if (!IsValid(InGamePhaseListener) || !InGamePhaseListener->GetClass()->ImplementsInterface(UGamePhaseListener::StaticClass()))
		return;

	//GamePhaseListeners.Remove(InGamePhaseListener);	// en com car au cas où ça poserait des pb de mémoire pendant le parcours de la liste en shipping
}

void AGameModeTankShared::RegisterPhysicsTickable(AActor* InPhysicsTickable)
{
	if (!IsValid(InPhysicsTickable) || !InPhysicsTickable->GetClass()->ImplementsInterface(UPhysicsTickableShared::StaticClass()))
		return;

	UE_LOGFMT(LogGP_A5_LittlePipoTank, Warning, "Add to register tickable");
	PhysicsTickables.AddUnique(InPhysicsTickable);
}

void AGameModeTankShared::UnregisterPhysicsTickable(AActor* InPhysicsTickable)
{
	if (!IsValid(InPhysicsTickable) || !InPhysicsTickable->GetClass()->ImplementsInterface(UPhysicsTickableShared::StaticClass()))
		return;

	//PhysicsTickables.Remove(InPhysicsTickable);	// en com car au cas où ça poserait des pb de mémoire pendant le parcours de la liste en shipping
}
