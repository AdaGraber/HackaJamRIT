// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerModifier.h"
#include "PlayerCharacter.generated.h"

UCLASS(Blueprintable)
class HACKAJAMRITPROJECT_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	// Player Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float JumpSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Defense = 100.0f;

	// TEMP
	UPROPERTY(EditAnywhere)
	TSubclassOf<UPlayerModifier> TestModifier;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Modifiers")
	void ApplyPlayerModifier(TSubclassOf<UPlayerModifier> Modifier);
};
