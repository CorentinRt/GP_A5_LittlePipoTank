// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GamePhasesData.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class GP_A5_LITTLEPIPOTANK_API UGamePhasesData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GamePhases")
	float PreGameDuration = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GamePhases")
	float InGameDuration = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GamePhases")
	float PostGameDuration = 5.f;
	
};
