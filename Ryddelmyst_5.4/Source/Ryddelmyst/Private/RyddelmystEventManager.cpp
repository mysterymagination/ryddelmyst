// Fill out your copyright notice in the Description page of Project Settings.


#include "RyddelmystEventManager.h"

void URyddelmystEventManager::ScheduleQuestCompletionEvent(AActor* ActorInWorld, float DelaySeconds, const FString& CompletionContext)
{
    ActorInWorld->GetWorldTimerManager().SetTimer(ScheduledEventTimerHandle, [&]() {
        UE_LOG(LogTemp, Warning, TEXT("scheduled quest completion broadcast"));
        QuestCompletionEvent.Broadcast(CompletionContext);
    }, DelaySeconds, false);
}