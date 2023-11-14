// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "IAttacker.h"
#include "MagicWeapon.h"
#include "SpellStaticMeshComponent.generated.h"

/**
 * A static mesh providing rendering and possible physics data for a bullet or spell effect
 */
UCLASS()
class RYDDELMYST_API USpellStaticMeshComponent : public UStaticMeshComponent, public IAttacker
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UMagicWeapon* Magic;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	AActor* Caster;
public:
	USpellStaticMeshComponent();
	UWeapon* GetWeapon_Implementation() override;
	FString GetCurrentAttack_Implementation() override;
	AActor* GetBattler_Implementation() override;
	void ExecuteAttack_Implementation
	(
		const FString& AttackName
	) override;
};
