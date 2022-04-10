// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OpenClose.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UOpenClose : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RYDDELMYST_API IOpenClose
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, category = "Interaction")
	void Open(AActor* Opener);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, category = "Interaction")
	void Close(AActor* Closer);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, category = "Interaction")
	FText Lock(AActor* Locker);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, category = "Interaction")
	FText Unlock(AActor* Unlocker);
};
