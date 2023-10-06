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
	UFUNCTION(BlueprintImplementableEvent)
	void OnProjectileHit(AProjectile* Projectile, AActor* Other); // ON HIT EFFECTS IMPLEMENTED DIRECTLY BY BLUEPRINTS

	// On Hit Modifiers
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ActorToSpawnOnHit;
};
