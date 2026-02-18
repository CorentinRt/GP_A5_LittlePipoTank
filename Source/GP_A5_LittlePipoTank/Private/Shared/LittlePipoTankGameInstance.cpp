// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/LittlePipoTankGameInstance.h"

const FString& ULittlePipoTankGameInstance::GetServerIp() const
{
	return ServerIp;
}

const FString& ULittlePipoTankGameInstance::GetPlayerName() const
{
	return PlayerName;
}

void ULittlePipoTankGameInstance::SetServerIp(FString NewIp)
{
	ServerIp = MoveTemp(NewIp);
}

void ULittlePipoTankGameInstance::SetPlayerName(FString NewPlayerName)
{
	PlayerName = MoveTemp(NewPlayerName);
}
