// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FBlueprintSessionSearchResult
{
	GENERATED_USTRUCT_BODY()

	FOnlineSessionSearchResult OnlineResult;

	UPROPERTY(BlueprintReadOnly)
	FString SessionID;
	UPROPERTY(BlueprintReadOnly)
	FString HostName;
	UPROPERTY(BlueprintReadOnly)
	int32 AvailableConnections;
	UPROPERTY(BlueprintReadOnly)
	int32 Ping;

public:

	FBlueprintSessionSearchResult();
	FBlueprintSessionSearchResult(FOnlineSessionSearchResult InOnlineResult);
};

/**
 * Subsystem for handling multiplayer sessions.
 */
UCLASS()
class HACKAJAMRITPROJECT_API UMultiplayerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	IOnlineSessionPtr SessionInterface;

	FOnlineSessionSettings SessionSettings;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	TArray<FOnlineSessionSearchResult> CurrentSearchResults;

	// Name of the session currently hosted by the local player.
	FName HostedSessionName;

public:

	UMultiplayerSubsystem();

	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	void CreateSession(FString SessionID);
	UFUNCTION(BlueprintCallable)
	void JoinSession(FString SessionID, const FBlueprintSessionSearchResult& Session);

	// Search for all active sessions. This function is asynchronous and does not return - must get CurrentSearchResults to see results.
	UFUNCTION(BlueprintCallable)
	void FindAllActiveSessions();
	UFUNCTION(BlueprintCallable)
	void FindSessionByName();

	void OnCreateSessionComplete(FName SessionID, bool bSuccess);
	void OnDestroySessionComplete(FName SessionID, bool bSuccess);
	void OnFindSessionsComplete(bool bSuccess);

	UFUNCTION(BlueprintCallable)
	FName GetSubsystemName();

	UFUNCTION(BlueprintCallable)
	TArray<FBlueprintSessionSearchResult> GetCurrentSearchResults();
};