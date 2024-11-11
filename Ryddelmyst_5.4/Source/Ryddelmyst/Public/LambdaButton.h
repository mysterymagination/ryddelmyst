// Copyright Jeff Creswell 2024. Subject to the GPL v3.0 license.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "LambdaButton.generated.h"

DECLARE_DELEGATE(FLambdaEvent);

/**
 * Button with a non-dynamic OnClick delegate so we can bind a lambda to it and perform arbitrary actions with arbitrary captures.
 */
UCLASS()
class RYDDELMYST_API ULambdaButton : public UButton
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FLambdaEvent LambdaEvent;

public:
	UFUNCTION()
	void ExecLambda();
};
