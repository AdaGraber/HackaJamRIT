// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Projectile.h"
#include "Net/UnrealNetwork.h"
#include "EnhancedInputComponent.h"
#include "Projectile.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

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
	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &APlayerCharacter::FireWeapon);
	}
}

void APlayerCharacter::FireWeapon_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White, "FireWeapon()");

	AProjectile* proj = GetWorld()->SpawnActor<AProjectile>(
			ProjectileClass, 
			WeaponComponent->GetComponentTransform()); /*WeaponComponent->GetSocketTransform(TEXT("Barrel"))*/

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "" + UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)
		->GetActorForwardVector().ToString());

	if(proj)
		proj->Setup(
			this, 
			GetController(), 
			UDamageType::StaticClass(), 
			GetActorForwardVector() * 2000);

	OnFireWeapon();
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

void APlayerCharacter::TakeDamageRep_Implementation(float DamageAmount, AController* EventInstigator, AActor* DamageCauser)
{
	//if(!HasAuthority()) return;

	Health -= DamageAmount;
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, Health);
}