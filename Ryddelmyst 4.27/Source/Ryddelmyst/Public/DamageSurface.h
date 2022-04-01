// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageSurface.generated.h"

// Applies damage of a given type to overlapping actors
UCLASS()
class RYDDELMYST_API ADamageSurface : public AActor
{
	GENERATED_BODY()

private:
	// Handle to the timer that controls the period at which damage is applied to actors in contact with this surface
	FTimerHandle DamageTimerHandle;

	// Data about the most recent overlap event with an Actor
	FHitResult HitResult;

	// Actor in contact with the surface. todo: make this a vector to support multiple contact actors
	UPROPERTY()
	AActor* ContactActor;

protected:
	// the damage dealt by the surface
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	TSubclassOf<UDamageType> DamageType;

	// defines the shape of the damage surface e.g. along the entirety of a static mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	UPrimitiveComponent* CollisionShapeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float DamageAmount;

public:	
	// Sets default values for this actor's properties
	ADamageSurface();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void ApplyDamage();
};
