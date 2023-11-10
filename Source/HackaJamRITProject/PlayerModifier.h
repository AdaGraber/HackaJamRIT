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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Description;

	// Player Modifiers
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MovementSpeedModifier = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float JumpSpeedModifier = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DefenseModifier = 1.0f;

	// Weapon Modifiers
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> ProjectileClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FireRateModifier = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int AdditionalProjectileCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AccuracyModifier = 1.0f;

	// Projectile Modifiers
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageModifier = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ProjectileSpeedModifier = 1.0f;

	// On Hit Modifiers
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ActorToSpawnOnHit;
};
