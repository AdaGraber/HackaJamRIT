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

	UPROPERTY(Replicated)
	TArray<FPlayerModifier> Boons; // All boons in the BoonTable; generated on construction

	UPROPERTY(Replicated)
	TArray<APlayerCharacter*> Players; // Players in the game

	UPROPERTY(Replicated)
	TArray<APlayerCharacter*> ActivePlayers; // Players active/alive in the game

	UPROPERTY(Replicated)
	TArray<APlayerCharacter*> PlayersReadyForNextRound;

private:

	void ReadBoonsFromTable();

public:

	UFUNCTION(Server, Reliable)
	void AddPlayer(APlayerCharacter* Player);
	UFUNCTION(Server, Reliable)
	void RemovePlayer(APlayerCharacter* Player);

	UFUNCTION(Server, Reliable)
	void AddActivePlayer(APlayerCharacter* Player);
	UFUNCTION(Server, Reliable)
	void AddInactivePlayer(APlayerCharacter* Player);

#pragma region BoonSelection
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void OnPlayerSelectedBoon(APlayerCharacter* Player);
#pragma endregion

private:

#pragma region Rounds
	UFUNCTION(Server, Reliable)
	void StartNextRound();
	UFUNCTION(Server, Reliable)
	void EndRound();
#pragma endregion

public:

	UFUNCTION(BlueprintCallable, Category = "Players")
	int GetActivePlayerCount() const;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};