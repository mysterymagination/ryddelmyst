// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExtremityCapsuleComponent.h"
#include "IAttacker.h"
#include "ClawCapsuleComponent.generated.h"

/**
 * 
 */
UCLASS()
class RYDDELMYST_API UClawCapsuleComponent : public UExtremityCapsuleComponent, public IAttacker
{
	GENERATED_BODY()
	UPROPERTY()
	UWeapon* Weapon;
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	UWeapon* GetWeapon_Implementation() { return Weapon; }
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	void ExecuteAttack_Implementation(
		UPrimitiveComponent* StrikingComp,
		AActor* StrickenActor,
		UPrimitiveComponent* StrickenComp,
		FVector NormalImpulse,
		const FHitResult& HitInfoconst,
		const FString& AttackName
	) { Weapon->AttackMap[AttackName]->OnHit(
			StrikingComp,
			StrickenActor,
			StrickenComp,
			NormalImpulse,
			HitInfoconst
		); 
	}
};
