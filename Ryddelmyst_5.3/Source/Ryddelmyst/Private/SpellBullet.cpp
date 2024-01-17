// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellBullet.h"

// Sets default values
ASpellBullet::ASpellBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	if (!HitBoxer)
	{
		HitBoxer = CreateDefaultSubobject<UHitBoxerComponent>(TEXT("SpellBulletHitBoxer"));
	}
	// todo: does it make sense to force a separate shape from the static mesh? Couldn't we set RootComponent to the static mesh if the static mesh asset is set, and then do the same physics and collision config for the static mesh?
	if (!BulletShape)
	{
		UE_LOG(LogTemp, Error, TEXT("SpellBullet is missing a defined spell shape; default unrendered USceneComponent will be used for transform root."));
		// Set the default root scene component.
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SpellBulletSceneRoot"));
	}
	else
	{
		// physics config
		BulletShape->SetMassOverrideInKg(NAME_None, Mass, true);
		BulletShape->SetSimulatePhysics(true);
		// collision config
		BulletShape->SetNotifyRigidBodyCollision(true);
		BulletShape->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		BulletShape->SetCollisionProfileName("Projectile");
		FScriptDelegate onHitDelegate;
		onHitDelegate.BindUFunction(HitBoxer, FName("OnHit"));
		BulletShape->OnComponentHit.Add(onHitDelegate);
		// set the root component to be the collision component.
		RootComponent = BulletShape;
	}
	if (!BulletMovement)
	{
		// Use this component to drive this projectile's movement.
		BulletMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("BulletMovement"));
		BulletMovement->SetUpdatedComponent(BulletShape);
		BulletMovement->InitialSpeed = 3000.0f;
		BulletMovement->MaxSpeed = 3000.0f;
		BulletMovement->bRotationFollowsVelocity = true;
		BulletMovement->bShouldBounce = false;
		BulletMovement->ProjectileGravityScale = 0.0f;
	}
	if (BulletParticles)
	{
		// todo: create niagara component from system and attach to scene root 
	}
	
}

// Called when the game starts or when spawned
void ASpellBullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpellBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

