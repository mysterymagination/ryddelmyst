// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Engine/Canvas.h" 
#include "FPSHUD.generated.h"

/**
 * 
 */
UCLASS()
class RYDDELMYST_API AFPSHUD : public AHUD
{
	GENERATED_BODY()

public:
	// Primary draw call for the HUD.
	virtual void DrawHUD() override;
protected:
	// This will be drawn at the center of the screen.
	UPROPERTY(EditDefaultsOnly)
	UTexture2D* CrosshairTexture;
};
