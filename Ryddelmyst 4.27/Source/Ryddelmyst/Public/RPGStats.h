// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGStats.generated.h"

/**
 * Class wrapping common RPG statistics e.g. strength, defense...
 */
UCLASS(BlueprintType, Blueprintable)
class RYDDELMYST_API URPGStats : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG")
	float Strength = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG")
	float Defense = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG")
	float Dexterity = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG")
	float Magic = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG")
	float Spirit = 10.f;
};
