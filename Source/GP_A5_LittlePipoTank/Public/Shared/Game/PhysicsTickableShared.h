// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PhysicsTickableShared.generated.h"

/**
 * 
 */
UINTERFACE(Blueprintable)
class GP_A5_LITTLEPIPOTANK_API UPhysicsTickableShared : public UInterface
{
	GENERATED_BODY()
};

class GP_A5_LITTLEPIPOTANK_API IPhysicsTickableShared
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnTickPhysics_Blueprint(float DeltaTime);
	
	virtual void RegisterTickable() = 0;
	virtual void UnregisterTickable() = 0;
};
