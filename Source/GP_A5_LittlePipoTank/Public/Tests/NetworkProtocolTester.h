// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NetworkProtocolTester.generated.h"

/**
 * 
 */
UCLASS()
class GP_A5_LITTLEPIPOTANK_API ANetworkProtocolTester : public AActor
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	void TestSerialize();
	
	UFUNCTION(BlueprintCallable)
	void TestDeserialize();

private:
	TArray<BYTE> TestByteArray;
};
