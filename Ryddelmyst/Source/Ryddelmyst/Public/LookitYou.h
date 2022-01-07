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
 * Interface providing methods to enable an external camera and request player input control
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
	virtual void EnableCamera(bool enable) = 0;
	/**
	 * Tells our LookitYou that we would like it to process player input if possible 
	 */
	virtual void TakeControl() = 0;
	/** 
	 * @return the world space position vector of the ILookitYou impl 
	 */
	virtual FVector GetLocation() = 0;
	/**
	 * @return the world space rotation of the ILookitYou impl
	 */
	virtual FRotator GetRotation() = 0;
};
