// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiplayerSubsystem.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "FindSessionsCallbackProxy.h"

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
	
}

void UMultiplayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if(Subsystem)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "" + Subsystem->GetSubsystemName().ToString());

		SessionInterface = Subsystem->GetSessionInterface();
		if(SessionInterface.IsValid())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "SESSION VALID");

			// Bind delegates to callback from session operations
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMultiplayerSubsystem::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMultiplayerSubsystem::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMultiplayerSubsystem::OnFindSessionsComplete);
		}
	}
}
void UMultiplayerSubsystem::Deinitialize()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "Deinitialize");

	if(!HostedSessionName.IsEqual("")) SessionInterface->DestroySession(HostedSessionName);
}

void UMultiplayerSubsystem::CreateSession(FString SessionID)
{
	//SessionSettings.bAllowInvites = true;
	//SessionSettings.bAllowJoinInProgress = true;
	//SessionSettings.bShouldAdvertise = true; // Public; visible to others
	//SessionSettings.NumPublicConnections = 4; // Max 4 Players
	//SessionSettings.bUseLobbiesIfAvailable = true; // May be required by Steam
	//SessionSettings.bUsesPresence = true; // May be required by Steam
	//SessionSettings.bAllowJoinViaPresence = true;
	//SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName().ToString().Equals("NULL"); // LAN matches occur when not using any particular subsystem
	//SessionSettings.Set(FName("SERVER_NAME"), SessionID, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	//// Host is self, name is SessionID, SessionSettings configured above
	//SessionInterface->CreateSession(0, FName(SessionID), SessionSettings);








	FName MySessionName = FName("Co-op Adventure Session Name");

	FOnlineSessionSettings SessionSettings1;

	SessionSettings1.bAllowJoinInProgress = true;
	SessionSettings1.bIsDedicated = false;
	SessionSettings1.bShouldAdvertise = true;
	SessionSettings1.NumPublicConnections = 2;
	SessionSettings1.bUseLobbiesIfAvailable = true;
	SessionSettings1.bUsesPresence = true;
	SessionSettings1.bAllowJoinViaPresence = true;
	bool IsLAN = false;
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		IsLAN = true;
	}
	SessionSettings1.bIsLANMatch = IsLAN;

	SessionInterface->CreateSession(0, MySessionName, SessionSettings1);

}
void UMultiplayerSubsystem::JoinSession(FString SessionID, const FBlueprintSessionSearchResult& Session)
{
	SessionInterface->JoinSession(0, FName(SessionID), Session.OnlineResult);
}

void UMultiplayerSubsystem::FindAllActiveSessions()
{
	// Use a default search
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 99;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	SessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName().ToString().Equals("NULL");

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}
void UMultiplayerSubsystem::FindSessionByName()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	bool IsLAN = false;
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		IsLAN = true;
	}
	SessionSearch->bIsLanQuery = IsLAN;
	SessionSearch->MaxSearchResults = 9999;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());

	FNamedOnlineSession* k = SessionInterface->GetNamedSession(FName("Co-op Adventure Session Name"));
	if(k) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "" + k->SessionName.ToString());
}

void UMultiplayerSubsystem::OnCreateSessionComplete(FName SessionID, bool bSuccess)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "" + FString::Printf(TEXT("OnCreateSession: %d"), bSuccess));

	if(!HostedSessionName.IsEqual("")) SessionInterface->DestroySession(HostedSessionName);

	HostedSessionName = SessionID;
}
void UMultiplayerSubsystem::OnDestroySessionComplete(FName SessionID, bool bSuccess)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "" + FString::Printf(TEXT("OnDestroySession: %d"), bSuccess));
}
void UMultiplayerSubsystem::OnFindSessionsComplete(bool bSuccess)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "" + FString::Printf(TEXT("OnFindSessions: %d"), bSuccess));
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "" + FString::FromInt(SessionSearch->SearchResults.Num()));

	// Set current search results to be read from outside the subsystem
	CurrentSearchResults = SessionSearch->SearchResults;

	for(FOnlineSessionSearchResult Result : CurrentSearchResults)
	{
		if(Result.IsValid())
		{
			FString SessionID = "N/A";
			if(!Result.Session.SessionSettings.Get(FName("SERVER_NAME"), SessionID))
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "l");;

			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "" + SessionID);
		}
	}
}

FName UMultiplayerSubsystem::GetSubsystemName()
{
	return IOnlineSubsystem::Get()->GetSubsystemName();
}

TArray<FBlueprintSessionSearchResult> UMultiplayerSubsystem::GetCurrentSearchResults()
{
	TArray<FBlueprintSessionSearchResult> Results;

	for(FOnlineSessionSearchResult Result : CurrentSearchResults)
		Results.Add(FBlueprintSessionSearchResult(Result));

	return Results;
}