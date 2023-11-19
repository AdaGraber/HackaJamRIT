// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSubsystem.h"
#include "OnlineSubsystem.h"

FBlueprintSessionSearchResult::FBlueprintSessionSearchResult()
{

}
FBlueprintSessionSearchResult::FBlueprintSessionSearchResult(FOnlineSessionSearchResult InOnlineResult)
{
	OnlineResult = InOnlineResult;

	OnlineResult.Session.SessionSettings.Get(FName("SERVER_NAME"), SessionID);
	HostName = OnlineResult.Session.OwningUserName;
	AvailableConnections = OnlineResult.Session.NumOpenPublicConnections;
	Ping = OnlineResult.PingInMs;
}

UMultiplayerSubsystem::UMultiplayerSubsystem()
{
	CurrentSessionName = FName("Escalation Game");
}

void UMultiplayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if(OnlineSubsystem)
	{
		FString SubsystemName = OnlineSubsystem->GetSubsystemName().ToString();
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "Subsystem: " + SubsystemName);

		SessionInterface = OnlineSubsystem->GetSessionInterface();
		if(SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMultiplayerSubsystem::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMultiplayerSubsystem::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMultiplayerSubsystem::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMultiplayerSubsystem::OnJoinSessionComplete);

			SessionInterface->OnRegisterPlayersCompleteDelegates.AddUObject(this, &UMultiplayerSubsystem::OnRegisterPlayers);
		}
	}
}

void UMultiplayerSubsystem::Deinitialize()
{
	SessionInterface->DestroySession(CurrentSessionName);
}

void UMultiplayerSubsystem::CreateServer(FName ServerName)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "CreateServer");

	if(ServerName.ToString().IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "Server name is empty!");
		CreateServerDelegate.Broadcast(false);
		return;
	}

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.NumPublicConnections = 2;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";

	SessionSettings.Set(FName("SERVER_NAME"), ServerName.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	CurrentSessionName = FName(ServerName);
	SessionInterface->CreateSession(0, CurrentSessionName, SessionSettings);
}
void UMultiplayerSubsystem::FindAllServers()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "FindServer");

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	SessionSearch->MaxSearchResults = 9999;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}
void UMultiplayerSubsystem::JoinServer(int Index)
{
	// Get search result
	FOnlineSessionSearchResult Result = SessionSearch->SearchResults[Index];

	if(Result.IsValid())
		SessionInterface->JoinSession(0, CurrentSessionName, Result);
}

void UMultiplayerSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, FString::Printf(TEXT("OnCreateSessionComplete: %d"), bWasSuccessful));

	CreateServerDelegate.Broadcast(bWasSuccessful);

	// Move the server to Lobby
	if(bWasSuccessful)
		GetWorld()->ServerTravel("/Game/EscalationContent/Maps/Lobby?listen");
}
void UMultiplayerSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, 
		FString::Printf(TEXT("OnDestroySessionComplete, SessionName: %s, Success: %d"), *SessionName.ToString(), bWasSuccessful));
}
void UMultiplayerSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if(!bWasSuccessful) return;

	TArray<FOnlineSessionSearchResult> Results = SessionSearch->SearchResults;

	// Wrap results for use in blueprint via delegate
	TArray<FBlueprintSessionSearchResult> BlueprintResults;
	for(FOnlineSessionSearchResult result : Results)
	{
		FString SessionID;
		result.Session.SessionSettings.Get(FName("SERVER_NAME"), SessionID);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "Server Found: " + SessionID);

		FBlueprintSessionSearchResult BPResult(result);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "SessionID: " + BPResult.SessionID);
		BlueprintResults.Add(FBlueprintSessionSearchResult(result));
	}
	FindServerDelegate.Broadcast(BlueprintResults);

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, FString::FromInt(Results.Num()) + " Sessions");
}
void UMultiplayerSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	JoinServerDelegate.Broadcast(Result == EOnJoinSessionCompleteResult::Success);

	if(Result == EOnJoinSessionCompleteResult::Success)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "Successfully joined session " + SessionName.ToString());

		// Try to travel to map hosted by server (lobby)
		FString Address = "";
		if(SessionInterface->GetResolvedConnectString(CurrentSessionName, Address))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "Address: " + Address);
			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if(PlayerController)
			{
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "OnJoinSessionComplete Failed " + SessionName.ToString());
	}
}

void UMultiplayerSubsystem::OnRegisterPlayers(FName SessionName, const TArray<FUniqueNetIdRef>& PlayerId, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "Registered: " + PlayerId[0]->ToString());
}

FString UMultiplayerSubsystem::GetSessionIdByIndex(int Index)
{
	FString SessionID;
	SessionSearch->SearchResults[Index].Session.SessionSettings.Get(FName("SERVER_NAME"), SessionID);
	return SessionID;
}
FName UMultiplayerSubsystem::GetSubsystemName()
{
	return IOnlineSubsystem::Get()->GetSubsystemName();
}