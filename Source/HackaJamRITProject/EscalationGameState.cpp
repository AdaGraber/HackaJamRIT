// Fill out your copyright notice in the Description page of Project Settings.


#include "EscalationGameState.h"
#include "Net/UnrealNetwork.h"
#include "Logging/LogMacros.h"


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

#pragma region Rounds
void AEscalationGameState::StartNextRound_Implementation()
{
	ActivePlayers.Empty();
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "AEscalationGameState::NextRound()");
	
	// NextRound is handled by the individual players
	
	for(APlayerCharacter* Player : PlayersReadyForNextRound)
	{
		if(IsValid(Player))
			Player->OnNextRound();
		else
		{
			Players.Remove(Player);
			ActivePlayers.Remove(Player);
		}
	}
}
void AEscalationGameState::EndRound_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "AEscalationGameState::EndRound()");

	ReadBoonsFromTable();

	TArray<FPlayerModifier> AvailableBoons;

	for(int i = 0; i < 3; i++)
	{
		//// Get random boon until boon isn't already listed
		//int boonIndex;
		//do
		//{
		//	boonIndex = rand() % Boons.Num();
		//}
		//while(AvailableBoons.Contains(Boons[boonIndex]));

		//// Add boon to list of boons available to player
		//AvailableBoons.Add(Boons[boonIndex]);

		AvailableBoons.Add(Boons[rand() % Boons.Num()]);
	}

	for(APlayerCharacter* Player : Players)
	{
		// Call player EndRound; implemented in Blueprint; handles dead and alive players
		if(IsValid(Player))
			Player->OnEndRound(AvailableBoons);
		else
		{
			Players.Remove(Player);
			ActivePlayers.Remove(Player);
		}
	}
}
#pragma endregion

#pragma region BoonSelection
void AEscalationGameState::OnPlayerSelectedBoon_Implementation(APlayerCharacter* Player)
{
	// Add ready player
	PlayersReadyForNextRound.Add(Player);

	// If all players are ready, start next round
	if(PlayersReadyForNextRound.Num() >= Players.Num())
	{
		StartNextRound();
		PlayersReadyForNextRound.Empty();
	}
		
}
#pragma endregion

int AEscalationGameState::GetPlayerCount()
{
	return Players.Num();
}
int AEscalationGameState::GetActivePlayerCount()
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