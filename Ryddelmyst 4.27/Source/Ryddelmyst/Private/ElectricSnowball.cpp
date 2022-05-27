// Fill out your copyright notice in the Description page of Project Settings.


#include "ElectricSnowball.h"
#include "LightningDamageType.h"
#include "StatusEffected.h"
#include "ShockedStatusEffect.h"
#include "RPGStats.h"

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


	// update magic cost for subclass
	MagicCost = 20.f;

	DamageType = ULightningDamageType::StaticClass();
}

void AElectricSnowball::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ASnowball::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	// elemental StatusEffect
	if(OtherActor::StaticClass()->ImplementsInterface(UStatusEffected::StaticClass()))
    {
        // todo: check that OtherActor implements the IRPGStats interface and iff he does then pull down the stats for modification by ShockedStatusEffect (modifications which will be removed when the shocked effect ends)
		if(OtherActor::StaticClass()->ImplementsInterface(URPGStats::StaticClass()))
		{
			// todo: does making OtherActor the outer/owner and then also adding the status effect to a uprop'd tarray inside OtherActor cause the StatusEffect to be doubly referenced by OtherActor?  I want the StatusEffect to be owned by the target TArray, which itself is owned by OtherActor; StatusEffects removed from that array should be garbage collected.
			UShockedStatusEffect* StatusEffect = NewObject<UShockedStatusEffect>(OtherActor, UShockedStatusEffect::StaticClass());
			StatusEffect->SetId("ShockedStatusEffect");
			StatusEffect->SetShockDuration(ShockDuration);
			UE_LOG(LogTemp, Warning, TEXT("OnHit; applying primary StatusEffect with id %s"), *StatusEffect->GetId());
			IStatusEffected::Execute_AddStatusEffect(OtherActor, StatusEffect);
			// todo: should we apply effect directly here or lean on IStatusEffected::ProcessStatusEffects()?  It may need to depend on whether we're in real time or turn based mode at the moment, or maybe we should always run the apply code and the real time/turn based game mode switch will have to hook into StatusEffect objects somehow to turn off timers and replace with turn counts and convert any elapsed time to elapsed turns.
			StatusEffect->OnEffectApplied(OtherActor);
		}
    }
}

