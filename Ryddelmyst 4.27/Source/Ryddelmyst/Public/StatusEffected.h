// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "StatusEffected.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UStatusEffected : public UInterface
{
	GENERATED_BODY()
};

/**
 * This interface applies to any Actor who can be subject to a StatusEffect and must therefore process them.
 */
class RYDDELMYST_API IStatusEffected
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	
};
