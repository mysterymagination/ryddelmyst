
#include "FireSnowballAttack.h"

const FString UFireSnowballAttack::ATTACK_NAME("FireSnowballAttack");

UFireSnowballAttack::UFireSnowballAttack()
{
    DamageTypesToWeightsMap = 
	{
		{UFireDamageType::StaticClass(), 1.f}
	};
    AttackName = UFireSnowballAttack::ATTACK_NAME;
    BasePower = 20.f;
}

void UFireSnowballAttack::OnHit_Implementation(AActor* StrikingBattler, UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo)
{
    Super::OnHit_Implementation(StrikingBattler, StrikingComp, StrickenActor, StrickenComp, NormalImpulse, HitInfo);
    // elemental StatusEffect
    if(StrickenActor->GetClass()->ImplementsInterface(UStatusEffected::StaticClass()))
    {
        // todo: does making OtherActor the outer/owner and then also adding the status effect to a uprop'd tarray inside OtherActor cause the StatusEffect to be doubly referenced by OtherActor?  I want the StatusEffect to be owned by the target TArray, which itself is owned by OtherActor; StatusEffects removed from that array should be garbage collected.
        UBurnedStatusEffect* StatusEffect = NewObject<UBurnedStatusEffect>(StrickenActor, UBurnedStatusEffect::StaticClass());
        StatusEffect->SetId("BurnedStatusEffect");
        StatusEffect->SetBurnDuration(BurnDuration);
        StatusEffect->SetBurnPeriod(BurnPeriod);
        // ongoing damage will be init calculated damage over 2
        StatusEffect->SetBurnDamage(Damage/2.f);
        UE_LOG(LogTemp, Warning, TEXT("OnHit; applying primary StatusEffect with id %s"), *StatusEffect->GetId());
        IStatusEffected::Execute_AddStatusEffect(StrickenActor, StatusEffect);
        // todo: should we apply effect directly here or lean on IStatusEffected::ProcessStatusEffects()?  It may need to depend on whether we're in real time or turn based mode at the moment, or maybe we should always run the apply code and the real time/turn based game mode switch will have to hook into StatusEffect objects somehow to turn off timers and replace with turn counts and convert any elapsed time to elapsed turns.
        StatusEffect->OnEffectApplied(StrickenActor);
    }
}


