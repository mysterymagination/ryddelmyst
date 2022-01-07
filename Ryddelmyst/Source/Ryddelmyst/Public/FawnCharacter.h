// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LookitYou.h"
#include "GameFramework/Character.h"
#include "FawnCharacter.generated.h"

UCLASS(Abstract)
class RYDDELMYST_API AFawnCharacter : public ACharacter
{
	GENERATED_BODY()

public:	
	virtual void OnLostFollower(ILookitYou* lookitYou) PURE_VIRTUAL(AFawnCharacter::OnLostFollower,);
};
