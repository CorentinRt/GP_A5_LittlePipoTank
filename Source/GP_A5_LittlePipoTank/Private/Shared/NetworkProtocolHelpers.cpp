// Fill out your copyright notice in the Description page of Project Settings.
#include "Public/Shared/NetworkProtocolHelpers.h"

#include <string>


void UNetworkProtocolHelpers::SerializeString(TArray<BYTE>& ByteArray, const FString& Value)
{
	std::string StdString(TCHAR_TO_UTF8(*Value));
	
	//std::string StdString(reinterpret_cast<const char*>(StringCast<UTF8CHAR>(*Value).Get()));

	size_t Size = StdString.size();

	SerializeArithmetic(ByteArray, Size);
	
	for (SIZE_T i = 0; i < Size; i++)
	{
		SerializeArithmetic(ByteArray, StdString[i]);
	}
	
}

FString UNetworkProtocolHelpers::DeserializeString(const TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset)
{
	size_t size = DeserializeArithmetic<SIZE_T>(ByteArray, Offset);
	
	std::string StdString;
	StdString.resize(size);
	
	for (SIZE_T i = 0; i < size; i++)
	{
		StdString[i] = DeserializeArithmetic<char>(ByteArray, Offset);
	}
	
	//return FString(StringCast<TCHAR>(StdString.c_str()));
	return FString(UTF8_TO_TCHAR(StdString.c_str()));
}
