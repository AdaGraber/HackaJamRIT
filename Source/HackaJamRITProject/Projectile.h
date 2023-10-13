// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Projectile.generated.h"

UCLASS()
class HACKAJAMRITPROJECT_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:

	// Components
	/** Collider */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class USphereComponent* Collider;

	/** Projectile Mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* Mesh;

	/** Projectile Movement Component */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovement;

public:	
	// Sets default values for this actor's properties
	AProjectile();
	AProjectile(
		AActor* InOwner, AController* InOwnerController, 
		float InDamage, TSubclassOf<UDamageType> InDamageType, 
		FVector InVelocity);

	UPROPERTY(Replicated)
	bool bIsSetup = false;

	UPROPERTY(Replicated)
	AActor* OwnerActor;
	UPROPERTY(Replicated)
	AController* OwnerController;

	UPROPERTY(Replicated)
	float Damage;
	UPROPERTY(Replicated)
	TSubclassOf<UDamageType> DamageType;

	UFUNCTION(Server, Reliable)
	void Setup(AActor* InOwner, AController* InOwnerController, 
		float InDamage, TSubclassOf<UDamageType> InDamageType, 
		FVector InVelocity);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};