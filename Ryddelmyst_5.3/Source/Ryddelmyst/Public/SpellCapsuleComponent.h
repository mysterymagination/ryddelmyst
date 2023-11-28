// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "IAttacker.h"
#include "MagicWeapon.h"
#include "SpellCapsuleComponent.generated.h"

/**
 * A capsule collision geometry for a bullet or spell effect
 */
UCLASS()
class RYDDELMYST_API USpellCapsuleComponent : public UCapsuleComponent, public IAttacker
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UMagicWeapon* Magic;
public:
	USpellCapsuleComponent();
	UWeapon* GetWeapon_Implementation() override;
	FString GetCurrentAttack_Implementation() override;
	AActor* GetBattler_Implementation() override;
	void ExecuteAttack_Implementation
	(
		const FString& AttackName
	) override;
};
