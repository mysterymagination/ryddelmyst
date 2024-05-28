// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "IAttacker.h"
#include "MagicWeapon.h"
#include "SpellSphereComponent.generated.h"

/**
 * A spherical collision geometry for a bullet or spell effect
 */
UCLASS()
class RYDDELMYST_API USpellSphereComponent : public USphereComponent, public IAttacker
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UMagicWeapon* Magic;
public:
	USpellSphereComponent();
	UWeapon* GetWeapon_Implementation() override;
	FString GetCurrentAttack_Implementation() override;
	FBattleStatsData GetBattleStats_Implementation() override;
	void ExecuteAttack_Implementation
	(
		const FString& AttackName
	) override;
};
