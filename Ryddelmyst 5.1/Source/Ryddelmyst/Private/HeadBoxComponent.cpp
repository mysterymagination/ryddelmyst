// Fill out your copyright notice in the Description page of Project Settings.


#include "HeadBoxComponent.h"

UHeadBoxComponent::UHeadBoxComponent()
{
    SetNotifyRigidBodyCollision(true);
	SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetCollisionProfileName("BlockAllDynamic");
}

UAnatomyUnit* UHeadBoxComponent::GetAnatomyUnit_Implementation() 
{
    return HeadUnit;
}

UArmor* UHeadBoxComponent::GetArmor_Implementation()
{
    return Armor;
}


