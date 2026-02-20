// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/TankGamePhase.h"
#include "UObject/Object.h"
#include "GamePhaseListener.generated.h"

/**
 * 
 */
UINTERFACE(Blueprintable)
class GP_A5_LITTLEPIPOTANK_API UGamePhaseListener : public UInterface
{
	GENERATED_BODY()
};

class GP_A5_LITTLEPIPOTANK_API IGamePhaseListener
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ReactOnGamePhaseChanged(ETankGamePhase InGamePhase);
};
