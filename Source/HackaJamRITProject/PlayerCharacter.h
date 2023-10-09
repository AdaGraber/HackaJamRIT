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

	// Components
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* WeaponComponent;

	//Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	// Player Status
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	float Health = 100.0f;

	// Player Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HealthMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float JumpSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Defense = 100.0f;

	// Weapon Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UClass* ProjectileClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireRate = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ProjectileCount = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Accuracy = 0.8f; // Bullet Spread (Angle: Degrees) = 90(1 - Accuracy)

	// Projectile Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ProjectileSpeed = 2000;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//FEventRef OnProjectileHit;

	// TEMP
	UPROPERTY(EditAnywhere)
	FPlayerModifier TestModifier;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Weapon")
	void FireWeapon();
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void OnSpawnProjectile();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Modifiers")
	void ApplyPlayerModifier(const FPlayerModifier& Modifier);

	UFUNCTION(Server, Reliable)
	void TakeDamageRep(float DamageAmount, AController* EventInstigator, AActor* DamageCauser);
	void Die();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game")
	void OnEndRound(const TArray<FPlayerModifier>& Boons);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};