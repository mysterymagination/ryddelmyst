// Fill out your copyright notice in the Description page of Project Settings.


#include "BodyCapsuleComponent.h"

UBodyCapsuleComponent::UBodyCapsuleComponent()
{
    SetNotifyRigidBodyCollision(true);
    SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SetCollisionProfileName("BlockAllDynamic");
}

UAnatomyUnit* UBodyCapsuleComponent::GetAnatomyUnit_Implementation()
{
    return CoreUnit;
}

UArmor* UBodyCapsuleComponent::GetArmor_Implementation()
{
    return Armor;
}

FBattleStatsData UBodyCapsuleComponent::GetBattleStats_Implementation()
{
    return IBattleStatsBearer::Execute_GetStats(GetOwner())->StatsData;
}