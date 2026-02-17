// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM()
enum class EENetAddressType: uint8
{
	IpV4,
	IpV6,
	Any
};
