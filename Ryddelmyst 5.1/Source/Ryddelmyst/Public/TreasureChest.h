// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TreasureChest.generated.h"

UCLASS()
class RYDDELMYST_API ATreasureChest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATreasureChest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (MustImplement = "Item"))
	TSubclassOf<UObject> Treasure;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (MustImplement = "Item"))
	TSubclassOf<UObject> Key;
};
