// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENetGameMode.h"
#include "enet6/enet.h"
#include "ENetServerGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GP_A5_LITTLEPIPOTANK_API AENetServerGameMode : public AENetGameMode
{
	GENERATED_BODY()

protected:
	virtual void Tick(float DeltaSeconds) override;
public:
	
	virtual void InitializeNetwork() override;

protected:
	
};
