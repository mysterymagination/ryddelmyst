// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Snowball.h"
#include "IceySnowball.generated.h"

/**
 * A Snowball spell that deals cold damage, surprise surprise.  It freezes the target for a few seconds.
 */
UCLASS()
class RYDDELMYST_API AIceySnowball : public ASnowball
{
	GENERATED_BODY()

public:
	AIceySnowball();
};
