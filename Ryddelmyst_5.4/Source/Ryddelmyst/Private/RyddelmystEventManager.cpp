// Fill out your copyright notice in the Description page of Project Settings.


#include "RyddelmystEventManager.h"
#include "TimerManager.h"
#include "RyddelmystGameInstance.h"
#include "Engine/GameInstance.h"

void URyddelmystEventManager::ScheduleQuestCompletionEvent(UGameInstance* GameInstance, float DelaySeconds, FString CompletionContext)
{
	GameInstance->GetTimerManager().SetTimer(ScheduledEventTimerHandle, [GameInstance, CompletionContext]() {
        UE_LOG(LogTemp, Warning, TEXT("test quest completion broadcast with test string %s"), *CompletionContext);
		auto* RyddelmystGameInstance = Cast<URyddelmystGameInstance>(GameInstance);
		UE_LOG(LogTemp, Warning, TEXT("test quest completion broadcast; gameinstance says %p"), RyddelmystGameInstance);
        auto* EventManager = RyddelmystGameInstance->GetEventManager();
		UE_LOG(LogTemp, Warning, TEXT("test quest completion broadcast; eventmanager says %p"), EventManager);
		UE_LOG(LogTemp, Warning, TEXT("test quest completion broadcast; questcompletionevent address says %p"), &EventManager->QuestCompletionEvent);
		EventManager->QuestCompletionEvent.Broadcast(CompletionContext);
    }, DelaySeconds, false);
}