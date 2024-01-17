// Fill out your copyright notice in the Description page of Project Settings.


#include "BodyBoxComponent.h"

UBodyBoxComponent::UBodyBoxComponent()
{
    SetNotifyRigidBodyCollision(true);
    SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SetCollisionProfileName("BlockAllDynamic");
}

UAnatomyUnit* UBodyBoxComponent::GetAnatomyUnit_Implementation() 
{
    return CoreUnit;
}

UArmor* UBodyBoxComponent::GetArmor_Implementation()
{
    return Armor;
}

AActor* UBodyBoxComponent::GetBattler_Implementation()
{
    return GetOwner();
}


