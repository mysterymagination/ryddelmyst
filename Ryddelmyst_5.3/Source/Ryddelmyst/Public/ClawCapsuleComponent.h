// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExtremityCapsuleComponent.h"
#include "IAttacker.h"
#include "ClawWeapon.h"
#include "ClawCapsuleComponent.generated.h"

/**
 * A UPrimitiveComponent representing the physical form of a creature's claw
 */
UCLASS(ClassGroup = "Collision", editinlinenew, Blueprintable, BlueprintType, meta = (DisplayName = "Claw Capsule Collision", BlueprintSpawnableComponent))
class RYDDELMYST_API UClawCapsuleComponent : public UExtremityCapsuleComponent, public IAttacker
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UClawWeapon* Claw;
public:
	UClawCapsuleComponent();
	AActor* GetBattleStats_Implementation() override;
	UWeapon* GetWeapon_Implementation() override;
	FString GetCurrentAttack_Implementation() override;
	void ExecuteAttack_Implementation
	(
		const FString& AttackName
	) override;
};
