// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LookitYou.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULookitYou : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RYDDELMYST_API ILookitYou
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * Tells the ILookitYou implementation to turn on its camera, typically because the calling entity is deactivating its own camera and wants to switch to an external view 
	 * @param enable indicates whether the LookitYou camera should be enabled or disabled
	 */
	void EnableCamera(bool enable) = 0;
};