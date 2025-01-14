// Fill out your copyright notice in the Description page of Project Settings.


#include "FireSnowball.h"
#include "FireDamageType.h"
#include "StatusEffected.h"
#include "FireSnowballAttack.h"
#include "BurnedStatusEffect.h"

AFireSnowball::AFireSnowball()
{
	// Use a fiery projectile material
	static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("'/Game/Ryddelmyst_Assets/Materials/FireSnowballMaterial.FireSnowballMaterial'"));
	if (Material.Succeeded())
	{
		ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, ProjectileMeshComponent);
	}
	ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);

	// Use a fiery particle system
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Game/Ryddelmyst_Assets/Particles/P_Fiery.P_Fiery"));
	if (ParticleAsset.Succeeded())
	{
		SnowballParticles->SetTemplate(ParticleAsset.Object);
	}

	// install this spell's attack
	IAttacker::Execute_GetWeapon(SpellSphereComponent)->AttackMap =
	{
		{UFireSnowballAttack::ATTACK_NAME, CreateDefaultSubobject<UFireSnowballAttack>(FName(*UFireSnowballAttack::ATTACK_NAME))}
	};
	IAttacker::Execute_GetWeapon(SpellSphereComponent)->CurrentAttackName = UFireSnowballAttack::ATTACK_NAME;
}
