
#include "ElectricSnowballAttack.h"
#include "LightningDamageType.h"
#include "StatusEffected.h"
#include "StatusEffect.h"
#include "ShockedStatusEffect.h"

const FString UElectricSnowballAttack::ATTACK_NAME("ElectricSnowballAttack");

UElectricSnowballAttack::UElectricSnowballAttack()
{
    // update magic cost for subclass
	Costs["MP"] = 20.f;

	DamageTypesToWeightsMap = 
	{
		{ULightningDamageType::StaticClass(), 1.f}
	};
    AttackName = UElectricSnowballAttack::ATTACK_NAME;
    BasePower = 15.f;
}

void UElectricSnowballAttack::OnHit_Implementation(AActor* StrikingBattler, UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo)
{
    Super::OnHit_Implementation(StrikingBattler, StrikingComp, StrickenActor, StrickenComp, NormalImpulse, HitInfo);
    // elemental StatusEffect
    if(StrickenActor->GetClass()->ImplementsInterface(UStatusEffected::StaticClass()))
    {
        // todo: we shouldn't assume the stricken AActor is our BattleStatsBearer or our StatusEffected guy; instead we should be pulling from StrickenComp by calling IDefender::Execute_GetBattler() on it
        if(StrickenActor->GetClass()->ImplementsInterface(UBattleStatsBearer::StaticClass()))
        {
            // todo: does making OtherActor the outer/owner and then also adding the status effect to a uprop'd tarray inside OtherActor cause the StatusEffect to be doubly referenced by OtherActor?  I want the StatusEffect to be owned by the target TArray, which itself is owned by OtherActor; StatusEffects removed from that array should be garbage collected.
            UShockedStatusEffect* StatusEffect = NewObject<UShockedStatusEffect>(StrickenActor, UShockedStatusEffect::StaticClass());
            StatusEffect->SetId("ShockedStatusEffect");
            StatusEffect->SetShockDuration(ShockDuration);
            UE_LOG(LogTemp, Warning, TEXT("OnHit; applying primary StatusEffect with id %s"), *StatusEffect->GetId());
            IStatusEffected::Execute_AddStatusEffect(StrickenActor, StatusEffect);
            // todo: should we apply effect directly here or lean on IStatusEffected::ProcessStatusEffects()?  It may need to depend on whether we're in real time or turn based mode at the moment, or maybe we should always run the apply code and the real time/turn based game mode switch will have to hook into StatusEffect objects somehow to turn off timers and replace with turn counts and convert any elapsed time to elapsed turns.
            StatusEffect->OnEffectApplied(StrickenActor);
        }
    }
}