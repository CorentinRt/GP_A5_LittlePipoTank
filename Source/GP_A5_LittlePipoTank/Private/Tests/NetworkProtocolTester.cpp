// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Tests/NetworkProtocolTester.h"

#include <rapidjson/document.h>

#include "HAL/MallocBinned2.h"
#include "Public/Shared/NetworkProtocolHelpers.h"

void ANetworkProtocolTester::TestSerialize()
{
	UINT8 MyUInt8 = 8;
	UINT16 MyUInt16 = 16;
	UINT32 MyUInt32 = 32;
	UINT64 MyUInt64 = 64;
	
	INT8 MyInt8 = 80;
	INT16 MyInt16 = 160;
	INT32 MyInt32 = 320;
	INT64 MyInt64 = 640;

	bool MyBool = true;

	float MyFloat32 = 32.32;
	double MyFloat64 = 64.64;
	
	UE_LOGFMT(LogTemp, Warning, "Test Serialize UINT8 : {0}", MyUInt8);
	UE_LOGFMT(LogTemp, Warning, "Test Serialize UINT16: {0}", MyUInt16);
	UE_LOGFMT(LogTemp, Warning, "Test Serialize UINT32: {0}", MyUInt32);
	UE_LOGFMT(LogTemp, Warning, "Test Serialize UINT64: {0}", MyUInt64);
	
	UE_LOGFMT(LogTemp, Warning, "Test Serialize INT8 : {0}", MyInt8);
	UE_LOGFMT(LogTemp, Warning, "Test Serialize INT16: {0}", MyInt16);
	UE_LOGFMT(LogTemp, Warning, "Test Serialize INT32: {0}", MyInt32);
	UE_LOGFMT(LogTemp, Warning, "Test Serialize INT64: {0}", MyInt64);
	
	UE_LOGFMT(LogTemp, Warning, "Test Serialize BOOL: {0}", MyBool);
		
	UE_LOGFMT(LogTemp, Warning, "Test Serialize FLOAT: {0}", MyFloat32);
	UE_LOGFMT(LogTemp, Warning, "Test Serialize DOUBLE: {0}", MyFloat64);
	
	UNetworkProtocolHelpers::SerializeArithmetic(TestByteArray, MyUInt8);
	UNetworkProtocolHelpers::SerializeArithmetic(TestByteArray, MyUInt16);
	UNetworkProtocolHelpers::SerializeArithmetic(TestByteArray, MyUInt32);
	UNetworkProtocolHelpers::SerializeArithmetic(TestByteArray, MyUInt64);
	
	UNetworkProtocolHelpers::SerializeArithmetic(TestByteArray, MyInt8);
	UNetworkProtocolHelpers::SerializeArithmetic(TestByteArray, MyInt16);
	UNetworkProtocolHelpers::SerializeArithmetic(TestByteArray, MyInt32);
	UNetworkProtocolHelpers::SerializeArithmetic(TestByteArray, MyInt64);
	
	UNetworkProtocolHelpers::SerializeArithmetic(TestByteArray, MyBool);
	
	UNetworkProtocolHelpers::SerializeArithmetic(TestByteArray, MyFloat32);
	UNetworkProtocolHelpers::SerializeArithmetic(TestByteArray, MyFloat64);
}

void ANetworkProtocolTester::TestDeserialize()
{
	TArray<BYTE>::SizeType Offset = 0;
	
	UINT8 MyUInt8 = UNetworkProtocolHelpers::DeserializeArithmetic<UINT8>(TestByteArray, Offset);
	UINT16 MyUInt16 = UNetworkProtocolHelpers::DeserializeArithmetic<UINT16>(TestByteArray, Offset);;
	UINT32 MyUInt32 = UNetworkProtocolHelpers::DeserializeArithmetic<UINT32>(TestByteArray, Offset);;
	UINT64 MyUInt64 = UNetworkProtocolHelpers::DeserializeArithmetic<UINT64>(TestByteArray, Offset);;
	
	INT8 MyInt8 = UNetworkProtocolHelpers::DeserializeArithmetic<INT8>(TestByteArray, Offset);;
	INT16 MyInt16 = UNetworkProtocolHelpers::DeserializeArithmetic<INT16>(TestByteArray, Offset);;
	INT32 MyInt32 = UNetworkProtocolHelpers::DeserializeArithmetic<INT32>(TestByteArray, Offset);;
	INT64 MyInt64 = UNetworkProtocolHelpers::DeserializeArithmetic<INT64>(TestByteArray, Offset);;

	bool MyBool = UNetworkProtocolHelpers::DeserializeArithmetic<bool>(TestByteArray, Offset);;

	float MyFloat32 = UNetworkProtocolHelpers::DeserializeArithmetic<float>(TestByteArray, Offset);;
	double MyFloat64 = UNetworkProtocolHelpers::DeserializeArithmetic<double>(TestByteArray, Offset);;

	UE_LOGFMT(LogTemp, Warning, "Test Deserialize UINT8 : {0}", MyUInt8);
	UE_LOGFMT(LogTemp, Warning, "Test Deserialize UINT16: {0}", MyUInt16);
	UE_LOGFMT(LogTemp, Warning, "Test Deserialize UINT32: {0}", MyUInt32);
	UE_LOGFMT(LogTemp, Warning, "Test Deserialize UINT64: {0}", MyUInt64);
	
	UE_LOGFMT(LogTemp, Warning, "Test Deserialize INT8 : {0}", MyInt8);
	UE_LOGFMT(LogTemp, Warning, "Test Deserialize INT16: {0}", MyInt16);
	UE_LOGFMT(LogTemp, Warning, "Test Deserialize INT32: {0}", MyInt32);
	UE_LOGFMT(LogTemp, Warning, "Test Deserialize INT64: {0}", MyInt64);
	
	UE_LOGFMT(LogTemp, Warning, "Test Deserialize BOOL: {0}", MyBool);
	
	UE_LOGFMT(LogTemp, Warning, "Test Deserialize FLOAT: {0}", MyFloat32);
	UE_LOGFMT(LogTemp, Warning, "Test Deserialize DOUBLE: {0}", MyFloat64);
}
