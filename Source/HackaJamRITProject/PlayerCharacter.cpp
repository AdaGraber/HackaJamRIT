// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	ApplyPlayerModifier(TestModifier);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlayerCharacter::ApplyPlayerModifier(TSubclassOf<UPlayerModifier> Modifier)
{
	if(Modifier == nullptr) return;

	UPlayerModifier* NewModifier = Modifier.GetDefaultObject();

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, 
		"APlayerCharacter::AddPlayerModifier: " + NewModifier->DisplayName);

	// Update Stats
	MovementSpeed *= NewModifier->MovementSpeedModifier;
	JumpSpeed *= NewModifier->JumpSpeedModifier;
	Damage *= NewModifier->DamageModifier;
}