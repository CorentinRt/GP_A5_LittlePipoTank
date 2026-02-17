// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LittlePipoTankGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class GP_A5_LITTLEPIPOTANK_API ULittlePipoTankGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	const FString& GetServerIp() const;

	UFUNCTION(BlueprintCallable)
	void SetServerIp(FString NewIp);
private:
	FString ServerIp = "localhost";
};
