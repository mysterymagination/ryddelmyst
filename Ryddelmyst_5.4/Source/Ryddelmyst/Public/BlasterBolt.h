// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpellCapsuleComponent.h"
#include "SpellBullet.h"
#include "BlasterBolt.generated.h"

/**
 * A spellbullet child with a capsule mesh, purple flame material, and blaster bolt attack. 
 */
UCLASS(ClassGroup = "Combat", Blueprintable, BlueprintType, meta = (DisplayName = "BlasterBolt", BlueprintSpawnableComponent))
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
};
