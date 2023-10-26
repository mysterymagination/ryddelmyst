// Fill out your copyright notice in the Description page of Project Settings.


#include "ExtremityCapsuleComponent.h"

UExtremityCapsuleComponent::UExtremityCapsuleComponent()
{
    SetNotifyRigidBodyCollision(true);
    SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SetCollisionProfileName("BlockAllDynamic");
}

UAnatomyUnit* UExtremityCapsuleComponent::GetAnatomyUnit_Implementation() 
{
    return ExUnit.Get();
}

UArmor* UExtremityCapsuleComponent::GetArmor_Implementation()
{
    return Armor.Get();
}

AActor* UExtremityCapsuleComponent::GetBattler_Implementation()
{
    return GetOwner();
}
