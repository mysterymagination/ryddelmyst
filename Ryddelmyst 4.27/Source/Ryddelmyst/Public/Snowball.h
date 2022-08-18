// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include <functional>
#include <vector>
#include "StatusEffect.h"
#include "IAttacker.h"
#include "Snowball.generated.h"

UCLASS()
class RYDDELMYST_API ASnowball : public AActor, public IAttacker
{
	GENERATED_BODY()

	// Vector of functions to be run on the target after the spell hits
	std::vector<std::function<void(AActor* TargetActor, const FHitResult& HitResult)>> EffectsOnTarget;
	// Function that provides custom launch behavior
	std::function<void(AActor* LaunchingActor, const FVector& LaunchDirection)> LaunchFn;
	UPROPERTY()
	FTimerHandle BrokenPhysicsTimerHandle;

public:
	// Sets default values for this actor's properties
	ASnowball();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Initializes the projectile's velocity in the shoot direction.
	void Cast(class ARyddelmystCharacter* CastingCharacter, const FVector& LaunchDirection);
	float GetMagicCost()
	{
		return MagicCost;
	};
	UFUNCTION()
	float GetDamageScaleFactor() { return DamageScaleFactor; };
	UFUNCTION()
	void SetDamageScaleFactor(float ScaleFactor) { DamageScaleFactor = ScaleFactor; };
	std::vector<std::function<void(AActor*, const FHitResult&)>>& GetEffectsVector() { return EffectsOnTarget; };
	std::function<void(AActor* LaunchingActor, const FVector& LaunchDirection)> GetLaunchFunction() { return LaunchFn; };
	void SetLaunchFunction(std::function<void(AActor* LaunchingActor, const FVector& LaunchDirection)> Function) { LaunchFn = Function; };
	UFUNCTION()
	void ProcessCost(class ARyddelmystCharacter* CasterCharacter);
	/**
	 * @brief Temporarily disables physics for this Snowball instance, for a duration of BrokenPhysicsPeriod
	 * 
	 */
	UFUNCTION()
	void BreakPhysics();

private:
	/**
	 * @brief re-enables simulated physics on this snowball
	 * 
	 */
	UFUNCTION()
	void FixPhysics();

protected:
	/**
	 * @brief Cache of most recently calculated damage
	 * 
	 */
	UPROPERTY()
	float Damage = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	TSubclassOf<UDamageType> DamageType;
	// Projectile particle FX
	UPROPERTY()
	UParticleSystemComponent* SnowballParticles;
	// Projectile material
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UMaterialInstanceDynamic* ProjectileMaterialInstance;
	// Projectile mesh
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UStaticMeshComponent* ProjectileMeshComponent;
	// Sphere collision component.
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComponent;
	// mana cost of this spell
	UPROPERTY(EditAnywhere, Category = Projectile)
	float MagicCost = 10.f;
	// base power used in CalculateDamageTx() to derive damage dealt by this spell
	UPROPERTY(EditAnywhere, Category = Projectile)
	float Power = 25.f;
	// scale factor for the final calculated damage
	UPROPERTY(EditAnywhere, Category = Projectile)
	float DamageScaleFactor = 1.f;
	/**
	 * @brief Time in seconds for which the Snowball should fly without physics enabled before re-enabling it
	 * 
	 */
	UPROPERTY(EditAnywhere, Category = Projectile)
	float BrokenPhysicsPeriod = 0.5f;

private:
	// Projectile movement component.
	UPROPERTY(VisibleAnywhere, Category = Movement)
	UProjectileMovementComponent* ProjectileMovementComponent;

protected:
	/**
	 * @brief Custom onhit event handling, applying any effects and damage, and then destroying the snowball 
	 * @param HitComp this is the UPrimitiveComponent doing the hitting, e.g. the snowball
	 * @param OtherActor this is the AActor instance we hit
	 * @param OtherComp this is the specific UPrimitiveComponent of the OtherActor that we hit
	 * @param NormalImpulse impulse vector of HitComp relative to OtherComp
	 * @param Hit FHitResult data about the collision
	 */
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	float CalculateDamageTx_Implementation(const FString& AttackName, AActor* BattleStatsBearer);
	const TArray<TSubclassOf<UDamageType>> GetDamageTypes_Implementation(const FString& AttackName) {TArray<TSubclassOf<UDamageType>> DamageTypeArray = {DamageType}; return DamageTypeArray;}
};
