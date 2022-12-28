// Fill out your copyright notice in the Description page of Project Settings.


#include "HeadBoxComponent.h"

UAnatomyUnit* UHeadBoxComponent::GetAnatomyUnit_Implementation() 
{
    return HeadUnit;
}

UArmor* UHeadBoxComponent::GetArmor_Implementation()
{
    return Armor;
}


