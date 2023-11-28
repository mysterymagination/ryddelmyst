// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectPtr.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
#include "HitBoxerComponent.h"
#include "Components/ShapeComponent.h"
#include "SpellStaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Materials/MaterialInstanceConstant.h"
#include "SpellBullet.generated.h"

UCLASS(ClassGroup = "Combat", Blueprintable, BlueprintType, meta = (DisplayName = "SpellBullet", BlueprintSpawnableComponent))
class RYDDELMYST_API ASpellBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpellBullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	/**
	 * @return The IAttacker implementer for this bullet
	 * 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	UObject* GetAttacker();
	virtual UObject* GetAttacker_Implementation();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision)
	TObjectPtr<UHitBoxerComponent> HitBoxer;
	// Projectile particle FX
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	TObjectPtr<UNiagaraSystem> BulletParticles;
	// Projectile material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	TObjectPtr<UMaterialInstanceConstant> BulletMaterial;
	// Projectile mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	TObjectPtr<USpellStaticMeshComponent> BulletMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float Mass = 0.5f;
	// Projectile movement component.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	TObjectPtr<UProjectileMovementComponent> BulletMovement;
};
