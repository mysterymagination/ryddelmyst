
#include "IcySnowballAttack.h"
#include "IceDamageType.h"
#include "StatusEffected.h"
#include "FrozenStatusEffect.h"
#include "BludgeoningDamageType.h"

const FString UIcySnowballAttack::ATTACK_NAME("IcySnowballAttack");

UIcySnowballAttack::UIcySnowballAttack()
{
    DamageTypesToWeightsMap = 
	{
		{UIceDamageType::StaticClass(), 0.75f},
        {UBludgeoningDamageType::StaticClass(), 0.25f}
	};
    AttackName = UIcySnowballAttack::ATTACK_NAME;
    BasePower = 10.f;
}

void UIcySnowballAttack::OnHit_Implementation(FBattleStatsData StrikingBattlerData, UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo)
{
    Super::OnHit_Implementation(StrikingBattlerData, StrikingComp, StrickenActor, StrickenComp, NormalImpulse, HitInfo);
    // todo: would be cleaner to install a functor with this elemental effect in the general SnowballAttack EffectsVector 
    // elemental StatusEffect
    if(StrickenActor->GetClass()->ImplementsInterface(UStatusEffected::StaticClass()))
    {
        // todo: does making StrickenActor the outer/owner and then also adding the status effect to a uprop'd tarray inside OtherActor cause the StatusEffect to be doubly referenced by OtherActor?  I want the StatusEffect to be owned by the target TArray, which itself is owned by OtherActor; StatusEffects removed from that array should be garbage collected.
        UFrozenStatusEffect* StatusEffect = NewObject<UFrozenStatusEffect>(StrickenActor, UFrozenStatusEffect::StaticClass());
        StatusEffect->SetId("FrozenStatusEffect");
        StatusEffect->SetFreezeDuration(FreezeDuration);
        UE_LOG(LogTemp, Warning, TEXT("OnHit; applying primary StatusEffect with id %s"), *StatusEffect->GetId());
        IStatusEffected::Execute_AddStatusEffect(StrickenActor, StatusEffect);
        // todo: should we apply effect directly here or lean on IStatusEffected::ProcessStatusEffects()?  It may need to depend on whether we're in real time or turn based mode at the moment, or maybe we should always run the apply code and the real time/turn based game mode switch will have to hook into StatusEffect objects somehow to turn off timers and replace with turn counts and convert any elapsed time to elapsed turns.
        StatusEffect->OnEffectApplied(StrickenActor);
    }
}