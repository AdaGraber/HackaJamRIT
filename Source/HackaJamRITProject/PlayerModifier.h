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

	UPROPERTY(EditAnywhere)
	FString DisplayName;
	UPROPERTY(EditAnywhere)
	FString Description;

	// Player Modifiers
	UPROPERTY(EditAnywhere)
	float MovementSpeedModifier = 1.0f;
	UPROPERTY(EditAnywhere)
	float JumpSpeedModifier = 1.0f;
	UPROPERTY(EditAnywhere)
	float DefenseModifier = 1.0f;

	// Weapon Modifiers
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ProjectileClass;
	UPROPERTY(EditAnywhere)
	float FireRateModifier = 1.0f;
	UPROPERTY(EditAnywhere)
	int AdditionalProjectileCount = 0;
	UPROPERTY(EditAnywhere)
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
