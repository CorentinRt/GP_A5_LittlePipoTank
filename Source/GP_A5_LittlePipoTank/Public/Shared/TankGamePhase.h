// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TankGamePhase.generated.h"

/**
 * 
 */
UENUM(Blueprintable, BlueprintType)
enum class ETankGamePhase : uint8
{
	WAITING_PLAYER = 0,
	PRE_GAME = 1,
	IN_GAME = 2,
	POST_GAME = 3,
	NONE = 4
};

