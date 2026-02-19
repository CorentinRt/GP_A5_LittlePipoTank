// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayersTankColorData.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class GP_A5_LITTLEPIPOTANK_API UPlayersTankColorData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Players Colors")
	TArray<TObjectPtr<UMaterialInterface>> TankColors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default Colors")
	TObjectPtr<UMaterialInterface> DefaultColor;
};
