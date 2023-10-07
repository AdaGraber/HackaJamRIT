// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Engine/DataTable.h"
#include "PlayerCharacter.h"
#include "EscalationGameState.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class HACKAJAMRITPROJECT_API AEscalationGameState : public AGameStateBase
{
	GENERATED_BODY()

	AEscalationGameState();

public:

	UPROPERTY(EditAnywhere)
	UDataTable* BoonTable;

private:

	TArray<FPlayerModifier*> Boons; // All boons in the BoonTable; generated on construction

	TArray<APlayerCharacter*> Players; // Players in the game

	TArray<APlayerCharacter*> ActivePlayers; // Players active/alive in the game

public:

	UFUNCTION(Server, Reliable)
	void AddInactivePlayer(APlayerCharacter* Player);

private:

	UFUNCTION(Server, Reliable)
	void EndRound();
};