// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include <functional>
#include <vector>
#include "StatusEffect.h"
#include "IAttacker.h"
#include "HitBoxerComponent.h"
#include "SpellSphereComponent.h"
#include "Snowball.generated.h"

UCLASS()
class RYDDELMYST_API ASnowball : public AActor
{
	GENERATED_BODY()

	UPROPERTY()
	FTimerHandle BrokenPhysicsTimerHandle;
	// Function that provides custom launch behavior
	std::function<void(AActor* LaunchingActor, const FVector& LaunchDirection)> LaunchFn;

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
	/**
	 * @brief Temporarily disables physics for this Snowball instance, for a duration of BrokenPhysicsPeriod
	 * 
	 */
	UFUNCTION()
	void BreakPhysics();
	/**
	 * Handler function registered with the OnComponentHit event attached to the collider shape (the SpellSphereComponent, our IAttacker). 
	 */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo);
	std::function<void(AActor* LaunchingActor, const FVector& LaunchDirection)> GetLaunchFunction() { return LaunchFn; };
	void SetLaunchFunction(std::function<void(AActor* LaunchingActor, const FVector& LaunchDirection)> Function) { LaunchFn = Function; };
	UFUNCTION()
	USpellSphereComponent* GetSpellSphereComponent()
	{
		return SpellSphereComponent;
	}

private:
	/**
	 * @brief re-enables simulated physics on this snowball
	 * 
	 */
	UFUNCTION()
	void FixPhysics();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision)
	UHitBoxerComponent* HitBoxer;
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
	USpellSphereComponent* SpellSphereComponent;
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
};
