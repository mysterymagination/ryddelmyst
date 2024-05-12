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
	// material setup
	if (BulletMaterial)
	{
		BulletMesh->SetMaterial(0, BulletMaterial);
	}
	// particle FX setup
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

UObject* ASpellBullet::GetAttacker_Implementation()
{
	return BulletMesh;
}
