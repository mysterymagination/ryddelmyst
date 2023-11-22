// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
#include "HitBoxerComponent.h"
#include "Components/ShapeComponent.h"
#include "SpellStaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SpellBullet.generated.h"

UCLASS()
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

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision)
	UHitBoxerComponent* HitBoxer;
	// Projectile particle FX
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	UNiagaraSystem* BulletParticles;
	// Projectile material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	UMaterialInterface* BulletMaterial;
	// Projectile mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	USpellStaticMeshComponent* BulletMesh;
	// Collision shape component.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	UShapeComponent* BulletShape;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float Mass = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	bool UseMeshForCollision = false;
private:
	// Projectile movement component.
	UPROPERTY(VisibleAnywhere, Category = Movement)
	UProjectileMovementComponent* BulletMovement;
};
