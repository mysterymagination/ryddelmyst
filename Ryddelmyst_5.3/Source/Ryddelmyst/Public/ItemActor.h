// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.h"
#include "ItemActor.generated.h"

UCLASS()
class RYDDELMYST_API AItemActor : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AItemActor();

public:	
	UFUNCTION()
	TSubclassOf<UObject> GetItemType();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (MustImplement = "Item"))
	TSubclassOf<UObject> ItemType;
};
