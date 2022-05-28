// Fill out your copyright notice in the Description page of Project Settings.


#include "FireSnowball.h"
#include "FireDamageType.h"
#include "StatusEffected.h"
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
	DamageType = UFireDamageType::StaticClass();
}

void AFireSnowball::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ASnowball::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	// elemental StatusEffect
	if(OtherActor->GetClass()->ImplementsInterface(UStatusEffected::StaticClass()))
    {
        // todo: does making OtherActor the outer/owner and then also adding the status effect to a uprop'd tarray inside OtherActor cause the StatusEffect to be doubly referenced by OtherActor?  I want the StatusEffect to be owned by the target TArray, which itself is owned by OtherActor; StatusEffects removed from that array should be garbage collected.
        UBurnedStatusEffect* StatusEffect = NewObject<UBurnedStatusEffect>(OtherActor, UBurnedStatusEffect::StaticClass());
        StatusEffect->SetId("BurnedStatusEffect");
        StatusEffect->SetBurnDuration(BurnDuration);
		StatusEffect->SetBurnPeriod(BurnPeriod);
		// ongoing damage will be init calculated damage over 2
		StatusEffect->SetBurnDamage(Damage/2.f);
        UE_LOG(LogTemp, Warning, TEXT("OnHit; applying primary StatusEffect with id %s"), *StatusEffect->GetId());
        IStatusEffected::Execute_AddStatusEffect(OtherActor, StatusEffect);
        // todo: should we apply effect directly here or lean on IStatusEffected::ProcessStatusEffects()?  It may need to depend on whether we're in real time or turn based mode at the moment, or maybe we should always run the apply code and the real time/turn based game mode switch will have to hook into StatusEffect objects somehow to turn off timers and replace with turn counts and convert any elapsed time to elapsed turns.
        StatusEffect->OnEffectApplied(OtherActor);
    }
}