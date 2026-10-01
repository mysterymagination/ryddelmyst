// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

// Sets default values
AItemActor::AItemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

TSubclassOf<UObject> AItemActor::GetItemType()
{
	return ItemType;
}

