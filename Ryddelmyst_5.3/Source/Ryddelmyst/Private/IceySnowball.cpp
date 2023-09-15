// Fill out your copyright notice in the Description page of Project Settings.


#include "IceySnowball.h"
#include "StatusEffected.h"
#include "IceDamageType.h"
#include "IcySnowballAttack.h"
#include "FrozenStatusEffect.h"

AIceySnowball::AIceySnowball()
{
    // icy material
    static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("'/Game/Ryddelmyst_Assets/Materials/SnowballMaterial.SnowballMaterial'"));
	if (Material.Succeeded())
	{
		ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, ProjectileMeshComponent);
	}
	ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);
    // icy particles
    static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Game/Ryddelmyst_Assets/Particles/P_Snowflakes.P_Snowflakes"));
	if (ParticleAsset.Succeeded())
	{
		SnowballParticles->SetTemplate(ParticleAsset.Object);
	}
	// install this spell's attack
	IAttacker::Execute_GetWeapon(SpellSphereComponent)->AttackMap =
	{
		{UIcySnowballAttack::ATTACK_NAME, CreateDefaultSubobject<UIcySnowballAttack>(FName(*UIcySnowballAttack::ATTACK_NAME))}
	};
	IAttacker::Execute_GetWeapon(SpellSphereComponent)->CurrentAttackName = UIcySnowballAttack::ATTACK_NAME;
}
