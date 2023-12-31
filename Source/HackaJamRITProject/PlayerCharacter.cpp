// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Projectile.h"
#include "Net/UnrealNetwork.h"
#include "EnhancedInputComponent.h"
#include "Projectile.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "EscalationGameState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"

#include <math.h>

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	// Create Head Collider
	HeadCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Head Collider"));
	HeadCollider->SetupAttachment(GetMesh(), TEXT("headSocket"));
	HeadCollider->SetRelativeScale3D(FVector(0.75f, 0.75f, 0.75f));

	// Create Camera Holder
	CameraHolder = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Holder"));
	CameraHolder->SetupAttachment(GetMesh());

	// Create Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Cam"));
	Camera->SetupAttachment(CameraHolder);
	Camera->bUsePawnControlRotation = true;

	//create camera holder TWO
	CameraHolder2 = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Holder2"));
	CameraHolder2->SetupAttachment(Camera);

	// Create FP Arms
	FPArms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Arms"));
	FPArms->SetupAttachment(CameraHolder2);

	// Create FP Weapon component
	FPWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Weapon"));
	

	// Create TP Weapon component
	TPWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TP_Weapon"));
	TPWeapon->SetupAttachment(GetMesh(), TEXT("RH_Weapon"));
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Add this player to server GameState's list of players
	AEscalationGameState* GameState = Cast<AEscalationGameState>(GetWorld()->GetGameState());
	GameState->AddPlayer(this);
	GameState->AddActivePlayer(this);

	Health = HealthMax;

	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	GetCharacterMovement()->JumpZVelocity = JumpSpeed;

	//FPWeapon->SetupAttachment(FPArms, "GripPoint");
	FPWeapon->AttachToComponent(FPArms, FAttachmentTransformRules::SnapToTargetIncludingScale, "GripPoint");
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
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &APlayerCharacter::TryFireWeapon);
	}
}

void APlayerCharacter::TryFireWeapon()
{
	if(!bCanFireWeapon) return;

	bCanFireWeapon = false;

	// Schedule bCanFireWeapon to be reset
	GetWorldTimerManager().SetTimer(
		FireWeaponTimerHandle, this, &APlayerCharacter::SetCanFireWeapon, 1.0f / FireRate, false);

	FireWeapon();
}

void APlayerCharacter::SetCanFireWeapon()
{
	bCanFireWeapon = true;
}

void APlayerCharacter::FireWeapon_Implementation()
{
	for(int i = 0; i < ProjectileCount; i++)
	{
		AProjectile* proj = GetWorld()->SpawnActor<AProjectile>(
			ProjectileClass, 
			FPWeapon->GetSocketTransform(TEXT("Muzzle")));
		
		FRotator WeaponRot = FPWeapon->GetSocketRotation(TEXT("Muzzle"));

		float BulletSpread = MAX_BULLET_SPREAD * (1 - Accuracy);

		FRotator Rotation(
			WeaponRot.Pitch + FMath::RandRange(0.0f, BulletSpread),
			WeaponRot.Yaw + FMath::RandRange(0.0f, BulletSpread),
			WeaponRot.Roll);

		FVector Dir = Rotation.Vector();

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

	Modifiers.Add(Modifier);

	// Update Player Stats
	MovementSpeed *= Modifier.MovementSpeedModifier;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	JumpSpeed *= Modifier.JumpSpeedModifier;
	GetCharacterMovement()->JumpZVelocity = JumpSpeed;
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
}

void APlayerCharacter::SelectBoon_Implementation(const FPlayerModifier& Modifier)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "APlayerCharacter::SelectBoon: " + Modifier.DisplayName);

	Modifiers.Add(Modifier);

	// Update Player Stats
	MovementSpeed *= Modifier.MovementSpeedModifier;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	JumpSpeed *= Modifier.JumpSpeedModifier;
	GetCharacterMovement()->JumpZVelocity = JumpSpeed;
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

	// Notify Server GameState that this player has selected a boon
	AEscalationGameState* GameState = Cast<AEscalationGameState>(GetWorld()->GetGameState());
	GameState->OnPlayerSelectedBoon(this);
}

void APlayerCharacter::TakeDamageRep_Implementation(
	float DamageAmount, AController* EventInstigator, AActor* DamageCauser, const TArray<UPrimitiveComponent*>& ComponentsHit, UActorComponent* ComponentHit)
{
	bool headshot = false;

	// headshot iff hits head collider component
	for(UActorComponent* component : ComponentsHit)
		if(component->GetFName() == TEXT("Head Collider")) headshot = true; // Uses FName because HeadCollider is sometimes null

	// If headshot, double damage
	if(headshot)
		DamageAmount *= 2.0f;

	Health -= DamageAmount * (100.0f / Defense);
	Health = FMath::Clamp(Health, 0, 100);

	// HitVector = opposite of Projectile direction
	FVector HitVector = -1 * DamageCauser->GetVelocity();
	HitVector = FVector(HitVector.X, HitVector.Y, 0);
	HitVector.Normalize();

	// Determine Hit Direction (relative to player's rotation; 2 directions)
	EDirection HitDirection = EDirection::Forward;
	float HitAngle = (HitVector.HeadingAngle() - GetActorForwardVector().HeadingAngle()) * 180 / PI;
	if(HitAngle > -90 && HitAngle < 90)
		HitDirection = EDirection::Forward;
	else if(HitAngle < -90 || HitAngle > 90)
		HitDirection = EDirection::Backward;

	if(Health <= 0) Die(HitDirection);
}

void APlayerCharacter::Die(EDirection HitDirection)
{
	// Do something; animation? -> Implemented in blueprint
	OnDie(HitDirection);

	AEscalationGameState* GameState = Cast<AEscalationGameState>(GetWorld()->GetGameState());

	// Register this player as dead in the Server GameState
	GameState->AddInactivePlayer(this);
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, Health);
}