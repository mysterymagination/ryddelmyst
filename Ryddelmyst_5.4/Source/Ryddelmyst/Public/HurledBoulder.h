// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpellBullet.h"
#include "HurledBoulder.generated.h"

/**
 * A large boulder hurled by a mighty creature. Very physics oriented. Mass is slightly randomized along with scale and damage is based on mass.
 */
UCLASS()
class RYDDELMYST_API AHurledBoulder : public ASpellBullet
{
	GENERATED_BODY()
	
public:
	AHurledBoulder();
	void OnHit(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo);	

protected:
	virtual void BeginPlay() override;
};
