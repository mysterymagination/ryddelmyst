// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Item.h"
#include "OpenClose.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UOpenClose : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for an Actor that can be opened or closed e.g. a container or a door
 */
class RYDDELMYST_API IOpenClose
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, category = "Interaction")
	void Open(AActor* Opener);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, category = "Interaction")
	void Close(AActor* Closer);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, category = "Interaction", meta = (MustImplement = "Item"))
	bool Lock(AActor* Locker, UObject* Item);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, category = "Interaction", meta = (MustImplement = "Item"))
	bool Unlock(AActor* Unlocker, UObject* Item);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, category = "Interaction")
	bool IsOpen();
};
