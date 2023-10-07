// Fill out your copyright notice in the Description page of Project Settings.


#include "EscalationGameState.h"

AEscalationGameState::AEscalationGameState()
{
	
}

void AEscalationGameState::AddInactivePlayer_Implementation(APlayerCharacter* Player)
{
	ActivePlayers.Remove(Player);

	if(ActivePlayers.Num() <= 1)
		EndRound();
}

void AEscalationGameState::EndRound_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "AEscalationGameState::EndRound()");

	BoonTable->GetAllRows<FPlayerModifier>(TEXT(""), Boons);

	TArray<FPlayerModifier> AvailableBoons;

	for(int i = 0; i < 3; i++)
	{
		AvailableBoons.Add(*Boons[rand() % Boons.Num()]);
	}

	for(APlayerCharacter* Player : Players)
	{
		Player->OnEndRound(AvailableBoons);
	}
}