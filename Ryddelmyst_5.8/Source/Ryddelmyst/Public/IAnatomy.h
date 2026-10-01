// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AnatomyUnit.h"
#include "IAnatomy.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAnatomy : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface through which an anatomical form, e.g. LegCapsuleComponent, can expose its function via a UAnatomyUnit object. 
 */
class RYDDELMYST_API IAnatomy
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Combat")
	UAnatomyUnit* GetAnatomyUnit();
};
