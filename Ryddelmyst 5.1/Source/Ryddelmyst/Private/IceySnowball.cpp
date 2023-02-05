// Fill out your copyright notice in the Description page of Project Settings.


#include "IceySnowball.h"
#include "StatusEffected.h"
#include "IceDamageType.h"
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
}
