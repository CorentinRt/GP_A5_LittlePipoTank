// Fill out your copyright notice in the Description page of Project Settings.
#include "Public/Shared/NetworkProtocolHelpers.h"


void UNetworkProtocolHelpers::SerializeString(TArray<BYTE>& ByteArray, const FString& Value)
{
	const char* String = TCHAR_TO_UTF8(*Value);
	SIZE_T size = strlen(String);

	SerializeArithmetic(ByteArray, size);

	for (SIZE_T i = 0; i < size; i++)
	{
		SerializeArithmetic(ByteArray, String[i]);
	}
	
}

FString UNetworkProtocolHelpers::DeserializeString(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset)
{
	/*
	SIZE_T size = DeserializeArithmetic<SIZE_T>(ByteArray, Offset);

	char Utf8String[size];
	
	for (SIZE_T i = 0; i < size; i++)
	{
		Utf8String[i] = DeserializeArithmetic<char>(ByteArray, Offset);
	}

	return FString(UTF8_TO_TCHAR(&Utf8String[0]));
*/
	return "";
}
