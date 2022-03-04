#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EDamageTypes : uint8
{
	EFire,
	ECold,
	EElectricity,
	EAcid,
	EForce,
	ESonic,
	EBludgeoning,
	EPiercing,
	ESlashing
};
