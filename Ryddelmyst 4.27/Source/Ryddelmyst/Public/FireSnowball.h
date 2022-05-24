// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Snowball.h"
#include "FireSnowball.generated.h"

/**
 * 
 */
UCLASS()
class RYDDELMYST_API AFireSnowball : public ASnowball
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AFireSnowball();
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};
