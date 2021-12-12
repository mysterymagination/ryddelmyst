// Fill out your copyright notice in the Description page of Project Settings.


#include "ElectricSnowball.h"

// Sets default values
AElectricSnowball::AElectricSnowball()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AElectricSnowball::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AElectricSnowball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

