// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Shared/TankGamePhase.h"
#include "GameModeTankServer.generated.h"

/**
 * 
 */
UCLASS()
class GP_A5_LITTLEPIPOTANK_API AGameModeTankServer : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	virtual void BeginPlay() override;


	void InitGameServer();

	UFUNCTION(BlueprintCallable)
	void SetGamePhase(ETankGamePhase GamePhase);

	UFUNCTION(BlueprintCallable)
	void NextGamePhase();


	UFUNCTION(BlueprintImplementableEvent)
	void ReactChangeGamePhase(ETankGamePhase InGamePhase);	// Blueprint implementable to handle timer in BP
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ETankGamePhase CurrentTankGamePhase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int PlayerCount = 0;

	bool IsServerInitialized = false;

private:
	void PlayerJoined();

	void PlayerLeft();
	
};
