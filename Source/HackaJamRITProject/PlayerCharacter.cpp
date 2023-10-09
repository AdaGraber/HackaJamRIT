// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Projectile.h"
#include "Net/UnrealNetwork.h"
#include "EnhancedInputComponent.h"
#include "Projectile.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "EscalationGameState.h"

#include <math.h>

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	// Create Weapon component
	//Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	//Weapon->SetupAttachment(GetMesh(), TEXT("weapon_r"));
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Add this player to server GameState's list of players
	AEscalationGameState* GameState = Cast<AEscalationGameState>(GetWorld()->GetGameState());

	GameState->AddPlayer(this);
	GameState->AddActivePlayer(this);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &APlayerCharacter::FireWeapon);
	}
}

void APlayerCharacter::FireWeapon_Implementation()
{
	for(int i = 0; i < ProjectileCount; i++)
	{
		AProjectile* proj = GetWorld()->SpawnActor<AProjectile>(
			ProjectileClass, 
			WeaponComponent->GetComponentTransform()); /*WeaponComponent->GetSocketTransform(TEXT("Barrel"))*/

		FVector Dir = GetActorForwardVector();
		//int BulletSpread = 30 * (1 - Accuracy);
		//Dir += GetActorRightVector() * cos(((rand() % BulletSpread) - 15) * (180 / PI)) * 0
		//	+ GetActorUpVector() *  sin(((rand() % BulletSpread) - 15) * (180 / PI)); // Random angle in up and right directions
		//Dir.Normalize();

		if(proj)
			proj->Setup(
				this, 
				GetController(), 
				Damage,
				UDamageType::StaticClass(), 
				Dir * ProjectileSpeed);

		OnSpawnProjectile();
	}
}

void APlayerCharacter::ApplyPlayerModifier_Implementation(const FPlayerModifier& Modifier)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, 
		"APlayerCharacter::AddPlayerModifier: " + Modifier.DisplayName);

	// Update Player Stats
	MovementSpeed *= Modifier.MovementSpeedModifier;
	JumpSpeed *= Modifier.JumpSpeedModifier;
	Defense *= Modifier.DefenseModifier;

	// Update Weapon Stats
	if(Modifier.ProjectileClass)
		ProjectileClass = Modifier.ProjectileClass;
	FireRate *= Modifier.FireRateModifier;
	ProjectileCount += Modifier.AdditionalProjectileCount;
	Accuracy *= Modifier.AccuracyModifier;

	// Update Projectile Stats
	Damage *= Modifier.DamageModifier;
	ProjectileSpeed *= Modifier.ProjectileSpeedModifier;

	//if(Modifier.ActorToSpawnOnHit)
		//
}

void APlayerCharacter::TakeDamageRep_Implementation(float DamageAmount, AController* EventInstigator, AActor* DamageCauser)
{
	//if(!HasAuthority()) return;

	Health -= DamageAmount;

	if(Health <= 0) Die();
}

void APlayerCharacter::Die()
{
	// Do something; animation?

	AEscalationGameState* GameState = Cast<AEscalationGameState>(GetWorld()->GetGameState());

	// Register this player as dead in the Server GameState
	GameState->AddInactivePlayer(this);
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, Health);
}