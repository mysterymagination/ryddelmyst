// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ConversationStarter.generated.h"

/**
 * A parser for JSON conversation data that generates Slate UI elements from a conversation script, including interactive dialogue trees.
 * This class is also responsible for doing the reverse -- writing a conversation replay JSON script so the player can review past conversations in the quest log;
 * these would essentially look like the final conversation UI after dialogue options have been picked and the actual talking head + speech bubble
 * have been rendered. Basically a modified version of the original conversation script with prefab chosen dialogue options instead of the directives to
 * offer choice button UI.
 */
UCLASS()
class RYDDELMYST_API UConversationStarter : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	UUserWidget* ParseConversationScript(const FString& Script);
	
};
