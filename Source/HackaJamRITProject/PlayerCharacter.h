// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerModifier.h"
#include "PlayerCharacter.generated.h"

UENUM(BlueprintType)
enum class EDirection : uint8
{
	Forward,
	Backward,
	Left,
	Right
};

UCLASS(Blueprintable)
class HACKAJAMRITPROJECT_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	const float MAX_BULLET_SPREAD = 15;

public:

	// Components
	/** Head Collider - detects headshots */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	class USphereComponent* HeadCollider;

	/** Camera Holder */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	class USpringArmComponent* CameraHolder;

	/** Camera Holder */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	class USpringArmComponent* CameraHolder2;

	/** Camera Component */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	class UCameraComponent* Camera;

	/** The FPS Armature */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	class USkeletalMeshComponent* FPArms;

	/** Weapon attached to the FPS Armature */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	class USkeletalMeshComponent* FPWeapon;

	/** Weapon attached to the TPS Armature */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	class USkeletalMeshComponent* TPWeapon;

	//Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* FireAction;

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	// Player Status
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	float Health;

	// Player Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HealthMax = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeed = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float JumpSpeed = 420.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Defense = 100.0f;

	// Weapon Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UClass* ProjectileClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireRate = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ProjectileCount = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Accuracy = 0.8f; // Bullet Spread (Angle: Degrees) = 15(1 - Accuracy)

	// Projectile Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ProjectileSpeed = 5000;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//FEventRef OnProjectileHit;

	// TEMP
	UPROPERTY(EditAnywhere)
	FPlayerModifier TestModifier;

private:

	bool bCanFireWeapon = true;
	void SetCanFireWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FTimerHandle FireWeaponTimerHandle;
	void TryFireWeapon();
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Weapon")
	void FireWeapon();
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void OnSpawnProjectile();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Modifiers")
	void ApplyPlayerModifier(const FPlayerModifier& Modifier);

	UFUNCTION(Server, Reliable)
	void TakeDamageRep(float DamageAmount, AController* EventInstigator, AActor* DamageCauser, UActorComponent* ComponentHit);
	void Die(EDirection HitDirection);
	UFUNCTION(BlueprintImplementableEvent, Category = "Damaging")
	void OnDie(EDirection HitDirection);

	UFUNCTION(BlueprintImplementableEvent, Category = "Game")
	void OnEndRound(const TArray<FPlayerModifier>& Boons);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};