// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Snowball.h"
#include "IceySnowball.generated.h"

/**
 * 
 */
UCLASS()
class RYDDELMYST_API AIceySnowball : public ASnowball
{
	GENERATED_BODY()
	
public:
	AIceySnowball();
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;	
};
