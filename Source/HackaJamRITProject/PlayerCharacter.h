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

	// Player Stats
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	float Health = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float JumpSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Defense = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UClass* ProjectileClass;

	// TEMP
	UPROPERTY(EditAnywhere)
	TSubclassOf<UPlayerModifier> TestModifier;

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
	void OnFireWeapon();

	UFUNCTION(BlueprintCallable, Category = "Modifiers")
	void ApplyPlayerModifier(TSubclassOf<UPlayerModifier> Modifier);

	UFUNCTION(Server, Reliable)
	void TakeDamageRep(float DamageAmount, AController* EventInstigator, AActor* DamageCauser);
	//float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};