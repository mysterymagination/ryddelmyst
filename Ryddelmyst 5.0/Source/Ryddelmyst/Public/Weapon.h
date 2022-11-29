// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Weapon.generated.h"

/**
 * An object representing a weapon of some sort, e.g. a sword or claw.  It has a list of attacks it can perform which include data such as the damage type an attack inflicts, the base damage value etc.
 * Weapons implement IAttacker such that we can have reusable objects with common default behavior logic whose results are modified by parameterized instances.  Weapons can then be associated with (equipped to)
 * AnatomyUnits.
 * todo: should this function be lumped under the anatomyunit functionality, or should it be paired with the form of the weapon itself?  Else how would situations where an extremity is holding a sword form
 * (e.g. some subclass of UPrimitiveComponent in a sword shape) but is not itself a sword form work?  How would we associate the sword form and function if the sword function lives in the functionality of
 * the hand holding it?  Seems convoluted and unhelpful; would be better to have related form and function objects directly associated the way our anatomy form and function objects are. 
 */
UCLASS(ClassGroup = "RPG", editinlinenew, Blueprintable, BlueprintType, meta = (DisplayName = "Weapon", BlueprintSpawnableComponent))
class RYDDELMYST_API UWeapon : public UObject, public IAttacker
{
	GENERATED_BODY()
	UPROPERTY()
    TMap<FString, TSubclassOf<UDamageType>> AttackDamageTypeMap;
public:
	void OnHit_Implementation(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo);
    TArray<TSubclassOf<UDamageType>> GetDamageTypes_Implementation(const FString& AttackName) { return AttackDamageTypeMap[AttackName]; }
    float CalculateDamageTx_Implementation(const FString& AttackName, AActor* BattleStatsBearer) { return 0.f; }
};
