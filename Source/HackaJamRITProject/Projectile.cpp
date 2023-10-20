// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "PlayerCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	Collider->SetupAttachment(RootComponent);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collider);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	bReplicates = true;
	SetReplicateMovement(true);
}

void AProjectile::Setup_Implementation(
	AActor* InOwner, AController* InOwnerController, 
	float InDamage, TSubclassOf<UDamageType> InDamageType, 
	FVector InVelocity)
{
	OwnerActor = InOwner;
	OwnerController = InOwnerController;
	Damage = InDamage;
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
	if(OtherActor == OwnerActor || OtherPlayer->GetController() == OwnerController) return; // Don't hit owner player

	// Get the component that was hit on the player
	UActorComponent* ComponentHit = nullptr;
	TSet<UPrimitiveComponent*> OverlappingComponents;
	GetOverlappingComponents(OverlappingComponents);
	for(UActorComponent* component : OverlappingComponents)
		if(component->GetOwner() == OtherActor) // If the component belongs to the player, set this as the HitComponent
			ComponentHit = component;

	// Apply damage to player (Server RPC), telling them which collider was hit
	OtherPlayer->TakeDamageRep(Damage, OwnerController, this, ComponentHit);
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