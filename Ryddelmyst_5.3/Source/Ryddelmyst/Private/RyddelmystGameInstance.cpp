// Fill out your copyright notice in the Description page of Project Settings.


#include "RyddelmystGameInstance.h"

URyddelmystGameInstance::URyddelmystGameInstance() : UGameInstance()
{
	EventManager = CreateDefaultSubobject<URyddelmystEventManager>(FName(TEXT("RyddelmystEventManager")));
}










