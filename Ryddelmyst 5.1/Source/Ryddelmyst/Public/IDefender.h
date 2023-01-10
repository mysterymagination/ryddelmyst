// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Containers/Map.h"
#include "CoreMinimal.h"
#include "Armor.h"
#include "UObject/Interface.h"
#include "IDefender.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDefender : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface allowing objects to supply data about their armor, which can in turn handle incoming hits 
 */
class RYDDELMYST_API IDefender
{
	GENERATED_BODY()

public:
	/**
     * @return the defender's armor data
     *
     */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	UArmor* GetArmor();
};
