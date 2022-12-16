// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Attack.generated.h"

/**
 * An Attack represents a particular attack that can be performed with a Weapon; it provides data and logic necessary to calculate damage and deliver additional effects to the stricken target.
 */
UCLASS(ClassGroup = "Combat", editinlinenew, Blueprintable, BlueprintType, meta = (DisplayName = "Attack", BlueprintSpawnableComponent))
class RYDDELMYST_API UAttack : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FString AttackName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<TSubclassOf<UDamageType>> DamageTypes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float BasePower;
public:
	/**
	 * @brief Processes a hit by this attacker on the given defender
	 * @param StrikingComp the component doing the hitting; this is the IAttacker impl
	 * @param StrickenActor the AActor we hit
	 * @param StrickenComp the UPrimitiveComponent we hit; this is the IDefender impl
	 * @param NormalImpulse the impulse vector normal to our strike point
	 * @param HitInfo collision data
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	void OnHit(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo);
	virtual void OnHit_Implementation(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo) {}
	/*
	todo: generate a ClawAttack class and give its OnHit_Implementation real behavior similar to:
	void UExtremityCapsuleComponent::OnHit_Implementation(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo)
{
    // todo: change this check to StrickenComp->GetArmor() and if non-null then go ahead and call the IDefender stuff on the Armor member; that way we can consolidate default behaviors for IAttacker and IDefender impls
    if (StrickenComp->GetClass()->ImplementsInterface(UDefender::StaticClass()))
    {
        // Damage setter is inside the IDefender target check so that we only bother calc/cache of damage if we can actually apply the damage
        UE_LOG(LogTemp, Warning, TEXT("OnHit; using attack name %s"), *ExUnit->GetCurrentAttack());
        float dmg = IAttacker::Execute_CalculateDamageTx(this, ExUnit->GetCurrentAttack(), StrikingComp->GetOwner());
        IDefender::Execute_CalculateDamageRx(StrickenComp, dmg, IAttacker::Execute_GetDamageTypes(this, ExUnit->GetCurrentAttack()));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OnHit; the stricken component %s does not implement IDefender, so we can't move forward communicating damage"), *StrickenComp->GetName());
    }
}
	*/
	/**
	 * @brief Calculates the base damage of our attack from the stats of the input IBattleStatsBearer implementor
	 * @param BattleStatsBearer the instigator of the attack, whose stats determine its damage output
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	float CalculateDamageTx(AActor* BattleStatsBearer);
	virtual float CalculateDamageTx_Implementation(AActor* BattleStatsBearer) { return 0.f; }
};
