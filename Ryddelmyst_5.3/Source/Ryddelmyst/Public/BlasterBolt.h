// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpellCapsuleComponent.h"
#include "SpellBullet.h"
#include "BlasterBolt.generated.h"

UCLASS()
class RYDDELMYST_API ABlasterBolt : public ASpellBullet
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlasterBolt();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	USpellCapsuleComponent* SpellCapsuleComponent;
};
