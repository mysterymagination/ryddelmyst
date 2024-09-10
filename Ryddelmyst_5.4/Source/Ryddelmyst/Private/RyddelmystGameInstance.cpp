// Fill out your copyright notice in the Description page of Project Settings.


#include "RyddelmystGameInstance.h"
#include "Sound/AmbientSound.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

URyddelmystGameInstance::URyddelmystGameInstance() : UGameInstance()
{
	EventManager = CreateDefaultSubobject<URyddelmystEventManager>(FName(TEXT("RyddelmystEventManager")));
}

void URyddelmystGameInstance::AdjustAmbientSounds(float ScaleFactor)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAmbientSound::StaticClass(), FoundActors);
	for (auto Actor : FoundActors)
	{
		UE_LOG(LogTemp, Log, TEXT("Adjusting music vol factor for %s to %f"), *Actor->GetName(), ScaleFactor);
		Cast<AAmbientSound>(Actor)->GetAudioComponent()->SetVolumeMultiplier(ScaleFactor);
	}
}








