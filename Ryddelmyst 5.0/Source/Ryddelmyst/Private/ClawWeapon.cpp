// Fill out your copyright notice in the Description page of Project Settings.


#include "ClawSlashAttack.h"
#include "ClawWeapon.h"

UClawWeapon::UClawWeapon()
{
    UE_LOG(LogTemp, Warning, TEXT("ctor clawweapon"));
    AttackMap = 
    {
        {UClawSlashAttack::ATTACK_NAME, CreateDefaultSubobject<UClawSlashAttack>(FName(*UClawSlashAttack::ATTACK_NAME))}
    };
}

