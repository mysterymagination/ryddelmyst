// Fill out your copyright notice in the Description page of Project Settings.


#include "RyddelmystGameInstance.h"
#include "Sound/AmbientSound.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

const FString URyddelmystGameInstance::TAG_FLAG_IGNORE_USER_VOL("IgnoreVolSlider");

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
		if (!Actor->Tags.Contains(URyddelmystGameInstance::TAG_FLAG_IGNORE_USER_VOL))
		{
			UE_LOG(LogTemp, Log, TEXT("Adjusting music vol factor for %s or %s to %f"), *Actor->GetName(), *Actor->GetActorNameOrLabel(), ScaleFactor);
			Cast<AAmbientSound>(Actor)->GetAudioComponent()->SetVolumeMultiplier(ScaleFactor);
		}
		else 
		{
			UE_LOG(LogTemp, Log, TEXT("Ignoring music vol %f for %s or %s"), ScaleFactor, *Actor->GetName(), *Actor->GetActorNameOrLabel());
		}
	}
}

bool URyddelmystGameInstance::CanSpawnMonster(TSubclassOf<AMonster> MonsterType)
{
	return CurrentMonsterCount < MaxMonsterCount;
}

void URyddelmystGameInstance::Pause()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetPause(true);
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->SetShowMouseCursor(true);
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PlayerController);
}

void URyddelmystGameInstance::Unpause()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetPause(false);
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->SetShowMouseCursor(false);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
}








