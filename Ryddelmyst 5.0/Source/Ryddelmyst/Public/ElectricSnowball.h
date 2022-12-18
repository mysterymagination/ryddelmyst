// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Snowball.h"
#include "ElectricSnowball.generated.h"

UCLASS()
class RYDDELMYST_API AElectricSnowball : public ASnowball
{
	GENERATED_BODY()
	
protected:
	// Duration during which the effected Actor will be shocked
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Magic)
	float ShockDuration = 5.f;
	
public:
	// Sets default values for this actor's properties
	AElectricSnowball();
	void OnSnowballHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	UFUNCTION()
	float GetShockDuration() { return ShockDuration; };
	UFUNCTION()
	void SetShockDuration(float Duration) { ShockDuration = Duration; };
};
