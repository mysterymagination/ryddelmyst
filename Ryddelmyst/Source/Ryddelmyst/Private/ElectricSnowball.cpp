// Fill out your copyright notice in the Description page of Project Settings.


#include "ElectricSnowball.h"

// Sets default values
AElectricSnowball::AElectricSnowball()
{
	// Use a shocking projectile material
	static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("'/Game/Ryddelmyst_Assets/Materials/ElectricSnowballMaterial.ElectricSnowballMaterial'"));
	if (Material.Succeeded())
	{
		ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, ProjectileMeshComponent);
	}
	ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);
	ProjectileMeshComponent->SetRelativeScale3D(FVector(0.09f, 0.09f, 0.09f));
	ProjectileMeshComponent->SetupAttachment(RootComponent);
	// Use a shocking particle system
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Game/Ryddelmyst_Assets/Particles/P_Lightning.P_Lightning"));
	if (ParticleAsset.Succeeded())
	{
		SnowballParticles->SetTemplate(ParticleAsset.Object);
	}
}

