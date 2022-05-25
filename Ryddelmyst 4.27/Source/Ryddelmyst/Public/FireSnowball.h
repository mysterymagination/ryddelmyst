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

protected:
	// Duration during which the effected Actor will be burned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Magic)
	float BurnDuration = 5.f;
	
public:
	// Sets default values for this actor's properties
	AFireSnowball();
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	UFUNCTION()
	float GetBurnDuration() { return BurnDuration; };
	UFUNCTION()
	void SetBurnDuration(float Duration) { BurnDuration = Duration; };
};
