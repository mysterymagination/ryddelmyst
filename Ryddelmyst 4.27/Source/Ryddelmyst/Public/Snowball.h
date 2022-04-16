// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include <functional>
#include <vector>
#include "Snowball.generated.h"

UCLASS()
class RYDDELMYST_API ASnowball : public AActor
{
	GENERATED_BODY()

	// Vector of functions to be run on the target after the spell hits
	std::vector<std::function<void(AActor*)>> EffectsOnTarget;

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
	void FireInDirection(const FVector& ShootDirection);
	float GetMagicCost()
	{
		return MagicCost;
	};
	UFUNCTION()
	float GetDamageScaleFactor() { return DamageScaleFactor; };
	UFUNCTION()
	void SetDamageScaleFactor(float ScaleFactor) { DamageScaleFactor = ScaleFactor; };
	std::vector<std::function<void(AActor*)>>& GetEffectsVector() { return EffectsOnTarget; };
	UFUNCTION()
	void SetCaster(class ARyddelmystCharacter* CastingCharacter) { Caster = CastingCharacter; };
	UFUNCTION()
	class ARyddelmystCharacter* GetCaster() { return Caster; };

protected:
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
	// base power used along with CalculateDamage() to derive damage dealt by this spell
	UPROPERTY(EditAnywhere, Category = Projectile)
	float Power = 25.f;
	// scale factor for the final calculated damage
	UPROPERTY(EditAnywhere, Category = Projectile)
	float DamageScaleFactor = 1.f;

private:
	UPROPERTY()
	class ARyddelmystCharacter* Caster;
	// Projectile movement component.
	UPROPERTY(VisibleAnywhere, Category = Movement)
	UProjectileMovementComponent* ProjectileMovementComponent;
	// Custom onhit event handling, destroying the snowball and spawning a flattened snowball actor at the collision point
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	// Calculates the damage dealt to target
	UFUNCTION()
	float CalculateDamage(class ARyddelmystCharacter* Character);
};
