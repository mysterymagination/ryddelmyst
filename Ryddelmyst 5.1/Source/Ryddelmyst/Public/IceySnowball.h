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

protected:
	// Duration during which the effected Actor will be unable to move
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float FreezeDuration = 5.f;	

public:
	AIceySnowball();
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	UFUNCTION()
	float GetFreezeDuration() { return FreezeDuration; };
	UFUNCTION()
	void SetFreezeDuration(float Duration) { FreezeDuration = Duration; };	
};
