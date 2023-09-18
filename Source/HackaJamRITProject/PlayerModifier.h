// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PlayerModifier.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class HACKAJAMRITPROJECT_API UPlayerModifier : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly)
	FString DisplayName;

	// Stat Modifiers
	UPROPERTY(EditDefaultsOnly)
	float MovementSpeedModifier = 1.0f;
	UPROPERTY(EditDefaultsOnly)
	float JumpSpeedModifier = 1.0f;
	UPROPERTY(EditDefaultsOnly)
	float DamageModifier = 1.0f;
	UPROPERTY(EditDefaultsOnly)
	float DefenseModifier = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ProjectileClass;

	// On Hit Modifiers
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ActorToSpawnOnHit;
};
