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
	
private:
	UPROPERTY()
	TScriptInterface<UItem> Item;

public:	
	// Sets default values for this actor's properties
	AItemActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UItem* GetItem();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TSubclassOf<UObject> ItemType;
};
