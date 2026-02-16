#pragma once

template<typename T> requires std::is_arithmetic_v<T>
T UNetworkProtocolHelpers::ByteSwap(T Value)
{
	if constexpr (sizeof(T) > sizeof(UINT64))
		UE_LOGFMT(LogTemp, Fatal, "Can't byte swap a type with a size > UINT64");
	
	if constexpr (FGenericPlatformProperties::IsLittleEndian())
	{
		if constexpr (sizeof(T) == sizeof(UINT64))
		{
			if constexpr (std::is_same_v<T, double>)
				Value = BYTESWAP_ORDERD(Value);
			else
				Value = BYTESWAP_ORDER64(Value);
		}
		else if constexpr (sizeof(T) == sizeof(UINT32))
		{
			if constexpr (std::is_same_v<T, float>)
				Value = BYTESWAP_ORDERF(Value);
			else
				Value = BYTESWAP_ORDER32(Value);
		}
		else if constexpr (sizeof(T) == sizeof(UINT16))
		{
			Value = BYTESWAP_ORDER16(Value);
		}
	}

	return Value;
}

template <typename T> requires std::is_arithmetic_v<T>
void UNetworkProtocolHelpers::SerializeArithmetic(TArray<BYTE>& ByteArray, T Value)
{
	if constexpr (sizeof(T) > sizeof(UINT64))
		UE_LOGFMT(LogTemp, Fatal, "Can't serialize a type with a size > UINT64");
		
	TArray<BYTE>::SizeType Offset = ByteArray.Num();
	ByteArray.AddUninitialized(sizeof(T));

	Value = ByteSwap(Value);

	FMemory::Memcpy(ByteArray.GetData() + Offset, &Value, sizeof(T));
}

template<typename T> requires std::is_arithmetic_v<T>
T UNetworkProtocolHelpers::DeserializeArithmetic(TArray<BYTE>& ByteArray, TArray<BYTE>::SizeType& Offset)
{
	T Value = T(0);

	FMemory::Memcpy(&Value,ByteArray.GetData() + Offset, sizeof(T));
	Offset += sizeof(T);

	Value = ByteSwap(Value);
	
	return Value;
}