// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Snowball.generated.h"

UCLASS()
class RYDDELMYST_API ASnowball : public AActor
{
	GENERATED_BODY()

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
	// todo: needs damage field
	// todo: needs EffectOnTarget functor field with an AActor* param so we can do things like have the diadem's fire metamagic both double the FireSnowball damage and set its effect to knock back the target

protected:
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

private:
	// Projectile movement component.
	UPROPERTY(VisibleAnywhere, Category = Movement)
	UProjectileMovementComponent* ProjectileMovementComponent;
	// Custom onhit event handling, destroying the snowball and spawning a flattened snowball actor at the collision point
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
