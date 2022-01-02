// Fill out your copyright notice in the Description page of Project Settings.


#include "FireSnowball.h"

AFireSnowball::AFireSnowball()
{
	// Use a fiery projectile material
	static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("'/Game/Ryddelmyst_Assets/Materials/FireSnowballMaterial.FireSnowballMaterial'"));
	if (Material.Succeeded())
	{
		ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, ProjectileMeshComponent);
	}
	ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);
	ProjectileMeshComponent->SetRelativeScale3D(FVector(0.09f, 0.09f, 0.09f));
	ProjectileMeshComponent->SetupAttachment(RootComponent);
	// Use a fiery particle system
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Game/Ryddelmyst_Assets/Particles/P_Fiery.P_Fiery"));
	if (ParticleAsset.Succeeded())
	{
		SnowballParticles->SetTemplate(ParticleAsset.Object);
	}
}