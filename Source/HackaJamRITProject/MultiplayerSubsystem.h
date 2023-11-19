// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCreateServerDelegate, bool, WasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFindServerDelegate, const TArray<FBlueprintSessionSearchResult>&, SearchResults);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FJoinServerDelegate, bool, WasSuccessful);

/**
 * 
 */
UCLASS()
class HACKAJAMRITPROJECT_API UMultiplayerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	IOnlineSessionPtr SessionInterface;

	FName CurrentSessionName;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

public:

	UMultiplayerSubsystem();

	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	void CreateServer(FName ServerName);
	UFUNCTION(BlueprintCallable)
	void FindAllServers();
	UFUNCTION(BlueprintCallable)
	void JoinServer(int Index);

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	void OnRegisterPlayers(FName SessionName, const TArray<FUniqueNetIdRef>& PlayerId, bool bWasSuccessful);

	UPROPERTY(BlueprintAssignable)
	FCreateServerDelegate CreateServerDelegate;
	UPROPERTY(BlueprintAssignable)
	FFindServerDelegate FindServerDelegate;
	UPROPERTY(BlueprintAssignable)
	FJoinServerDelegate JoinServerDelegate;

	UFUNCTION(BlueprintCallable)
	FString GetSessionIdByIndex(int Index);
	UFUNCTION(BlueprintCallable)
	FName GetSubsystemName();
};