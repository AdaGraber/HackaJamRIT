// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "PlayerCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	bReplicates = true;
	SetReplicateMovement(true);
}

void AProjectile::Setup_Implementation(
	AActor* InOwner, AController* InOwnerController, TSubclassOf<UDamageType> InDamageType, FVector InVelocity)
{
	OwnerActor = InOwner;
	OwnerController = InOwnerController;
	DamageType = InDamageType;

	if(ProjectileMovement) ProjectileMovement->Velocity = InVelocity;

	bIsSetup = true;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	// Only want server to do overlap and only if the projectile is set up
	if(!HasAuthority() || !bIsSetup) return;

	APlayerCharacter* OtherPlayer = Cast<APlayerCharacter>(OtherActor);

	if(!OtherPlayer) return;
	if(OtherActor == OwnerActor) return;
	if(OtherPlayer->GetController() == OwnerController) return; // Don't hit owner player

	OtherPlayer->TakeDamageRep(10, OwnerController, this);
	Destroy();
}

void AProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProjectile, bIsSetup);
	DOREPLIFETIME(AProjectile, OwnerActor);
	DOREPLIFETIME(AProjectile, OwnerController);
	DOREPLIFETIME(AProjectile, DamageType);
}