// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageTypes.h"
#include "DamageSurface.generated.h"

// Applies damage of a given type to overlapping actors
UCLASS()
class RYDDELMYST_API ADamageSurface : public AActor
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	EDamageTypes DamageType;

public:	
	// Sets default values for this actor's properties
	ADamageSurface();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
