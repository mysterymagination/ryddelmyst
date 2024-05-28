// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpellBullet.h"
#include "SpellSphereComponent.h"
#include "FlameBeadBullet.generated.h"

/**
 * A tiny sphere of fire suitable for bursts of bullets!
 */
UCLASS(ClassGroup = "Combat", Blueprintable, BlueprintType, meta = (DisplayName = "FlameBeadBullet", BlueprintSpawnableComponent))
class RYDDELMYST_API AFlameBeadBullet : public ASpellBullet
{
	GENERATED_BODY()

public:
	AFlameBeadBullet();
	virtual UObject* GetAttacker_Implementation();
protected:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OnHit(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo);
public:
	// Sphere collision component.
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USpellSphereComponent* SpellSphereComponent;
};
