// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterBoltAttack.h"
#include "FireDamageType.h"

const FString UBlasterBoltAttack::ATTACK_NAME("BlasterBoltAttack");

UBlasterBoltAttack::UBlasterBoltAttack()
{
	DamageTypesToWeightsMap =
	{
		{UFireDamageType::StaticClass(), 1.f}
	};
	AttackName = ATTACK_NAME;
	BasePower = 10.f;
}


