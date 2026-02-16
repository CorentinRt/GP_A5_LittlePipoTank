// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/Game/GameModeTankShared.h"

#include "Kismet/GameplayStatics.h"
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
	for (AActor* Tickable : PhysicsTickables)
	{
		if (!Tickable || !Tickable->GetClass()->ImplementsInterface(UPhysicsTickableShared::StaticClass()))
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

void AGameModeTankShared::ReactChangeGamePhase(ETankGamePhase InGamePhase)
{
	for (AActor* GamePhaseListener : GamePhaseListeners)
	{
		if (!GamePhaseListener || !GamePhaseListener->GetClass()->ImplementsInterface(UGamePhaseListener::StaticClass()))
			continue;

		IGamePhaseListener::Execute_ReactOnGamePhaseChanged(GamePhaseListener, InGamePhase);
	}
	
	ReactChangeGamePhase_Implementation(InGamePhase);
}

void AGameModeTankShared::RegisterListener(AActor* InGamePhaseListener)
{
	if (!InGamePhaseListener || !InGamePhaseListener->GetClass()->ImplementsInterface(UGamePhaseListener::StaticClass()))
		return;

	GamePhaseListeners.AddUnique(InGamePhaseListener);
}

void AGameModeTankShared::UnregisterListener(AActor* InGamePhaseListener)
{
	if (!InGamePhaseListener || !InGamePhaseListener->GetClass()->ImplementsInterface(UGamePhaseListener::StaticClass()))
		return;

	GamePhaseListeners.Remove(InGamePhaseListener);
}

void AGameModeTankShared::RegisterPhysicsTickable(AActor* InPhysicsTickable)
{
	if (!InPhysicsTickable || !InPhysicsTickable->GetClass()->ImplementsInterface(UPhysicsTickableShared::StaticClass()))
		return;

	PhysicsTickables.AddUnique(InPhysicsTickable);
}

void AGameModeTankShared::UnregisterPhysicsTickable(AActor* InPhysicsTickable)
{
	if (!InPhysicsTickable || !InPhysicsTickable->GetClass()->ImplementsInterface(UPhysicsTickableShared::StaticClass()))
		return;

	PhysicsTickables.Remove(InPhysicsTickable);
}
