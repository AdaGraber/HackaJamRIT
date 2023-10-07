// Fill out your copyright notice in the Description page of Project Settings.


#include "EscalationGameState.h"
#include "Net/UnrealNetwork.h"

AEscalationGameState::AEscalationGameState()
{
	bReplicates = true;
}

void AEscalationGameState::ReadBoonsFromTable()
{
	TArray<FPlayerModifier*> OutBoons;
	BoonTable->GetAllRows<FPlayerModifier>(TEXT(""), OutBoons);

	// Dereference all for use in replication
	for(FPlayerModifier* boon : OutBoons)
	{
		Boons.Add(*boon);
	}
}

void AEscalationGameState::AddPlayer_Implementation(APlayerCharacter* Player)
{
	Players.Add(Player);
}
void AEscalationGameState::RemovePlayer_Implementation(APlayerCharacter* Player)
{
	Players.Remove(Player);
}

void AEscalationGameState::AddActivePlayer_Implementation(APlayerCharacter* Player)
{
	ActivePlayers.Add(Player);
}
void AEscalationGameState::AddInactivePlayer_Implementation(APlayerCharacter* Player)
{
	ActivePlayers.Remove(Player);

	if(ActivePlayers.Num() <= 1)
		EndRound();
}

void AEscalationGameState::EndRound_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "AEscalationGameState::EndRound()");

	ReadBoonsFromTable();

	TArray<FPlayerModifier> AvailableBoons;

	for(int i = 0; i < 3; i++)
	{
		AvailableBoons.Add(Boons[rand() % Boons.Num()]);
	}

	for(APlayerCharacter* Player : Players)
	{
		Player->OnEndRound(AvailableBoons);
	}
}

int AEscalationGameState::GetActivePlayerCount() const
{
	return ActivePlayers.Num();
}

void AEscalationGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEscalationGameState, Boons);
	DOREPLIFETIME(AEscalationGameState, Players);
	DOREPLIFETIME(AEscalationGameState, ActivePlayers);
}