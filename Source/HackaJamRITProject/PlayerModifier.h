// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "PlayerModifier.generated.h"

/**
 * 
 */
USTRUCT(Blueprintable)
struct HACKAJAMRITPROJECT_API FPlayerModifier : public FTableRowBase
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly)
	FString DisplayName;

	// Player Modifiers
	UPROPERTY(EditDefaultsOnly)
	float MovementSpeedModifier = 1.0f;
	UPROPERTY(EditDefaultsOnly)
	float JumpSpeedModifier = 1.0f;
	UPROPERTY(EditDefaultsOnly)
	float DefenseModifier = 1.0f;

	// Weapon Modifiers
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ProjectileClass;

	// Projectile Modifiers
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageModifier = 1.0f;

	// On Hit Modifiers
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ActorToSpawnOnHit;
};
