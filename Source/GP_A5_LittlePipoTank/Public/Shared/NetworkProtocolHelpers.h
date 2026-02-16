// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NetworkProtocolHelpers.generated.h"

/**
 * 
 */
UCLASS()
class GP_A5_LITTLEPIPOTANK_API UNetworkProtocolHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	template<typename T> requires std::is_arithmetic_v<T>
	static T ByteSwap(T Value);
	
	template<typename T> requires std::is_arithmetic_v<T>
	static void SerializeArithmetic(TArray<BYTE>& ByteArray, T Value);
	template<typename T> requires std::is_arithmetic_v<T>
	static T DeserializeArithmetic(TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset);
	
};

#if CPP
	#include "NetworkProtocolHelpers.inl"
#endif