// Fill out your copyright notice in the Description page of Project Settings.

#include "ClawWeapon.h"
#include "ClawSlashAttack.h"

UClawWeapon::UClawWeapon()
{
    UE_LOG(LogTemp, Warning, TEXT("ctor clawweapon"));
    AttackMap = 
    {
        {UClawSlashAttack::ATTACK_NAME, CreateDefaultSubobject<UClawSlashAttack>(FName(*UClawSlashAttack::ATTACK_NAME))}
    };


    // todo: remove once we have AI behavior setting this value
    CurrentAttackName = UClawSlashAttack::ATTACK_NAME;
}

