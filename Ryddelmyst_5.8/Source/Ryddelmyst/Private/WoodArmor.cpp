// Fill out your copyright notice in the Description page of Project Settings.


#include "WoodArmor.h"

UWoodArmor::UWoodArmor()
{
    DamageReductionMap = 
    {
        {EDamageCat::Physical, 0.75f},
        {EDamageCat::Magical, 0.25f}
    };
}
