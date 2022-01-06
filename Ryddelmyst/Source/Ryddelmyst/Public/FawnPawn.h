// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LookitYou.h"
#include "GameFramework/Pawn.h"
#include "FawnPawn.generated.h"

UCLASS(Abstract)
class RYDDELMYST_API AFawnPawn : public APawn
{
	GENERATED_BODY()

public:	
	virtual void OnLostFollower(ILookitYou* lookitYou) PURE_VIRTUAL(AFawnPawn::OnLostFollower,);
};
