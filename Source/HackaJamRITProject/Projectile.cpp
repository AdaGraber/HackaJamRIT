// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AProjectile::Setup(AController* InOwnerController, TSubclassOf<UDamageType> InDamageType)
{
	OwnerController = InOwnerController;
	DamageType = InDamageType;
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
	FDamageEvent DamageEvent(DamageType);

	OtherActor->TakeDamage(100, DamageEvent, OwnerController, this);
}