// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/LittlePipoTankGameInstance.h"

const FString& ULittlePipoTankGameInstance::GetServerIp() const
{
	return ServerIp;
}

void ULittlePipoTankGameInstance::SetServerIp(FString NewIp)
{
	ServerIp = MoveTemp(NewIp);
}
