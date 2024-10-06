// Copyright Jeff Creswell 2024. Subject to the GPL v3.0 license.


#include "ConversationalComponent.h"

// Sets default values for this component's properties
UConversationalComponent::UConversationalComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	ConversationStarter = CreateDefaultSubobject<UConversationStarter>(TEXT("ConvoStarter"));
}


// Called when the game starts
void UConversationalComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UConversationalComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FString UConversationalComponent::GetConversationScript_Implementation(const FString& ConvoTx, const FString& ConvoRx, FName ClosestBone, ARyddelmystGameState* GameState)
{
	// todo: use game state to find best fit script
	FString ConvoPath = FPaths::ProjectContentDir().Append(TEXT("Ryddelmyst_Assets/Text/Dialogue/"));
	FString ChosenScript;
	FString ConvoJSON;
	IFileManager& FileManager = IFileManager::Get();
	TArray<FString> ConvoScriptFiles;
	// enumerate list of scripts
	FileManager.FindFiles(ConvoScriptFiles, *ConvoPath, TEXT("*.json"));
	// filter list by convo tx and rx, particularly rx goddess form
	// at the moment, we only ever have Maya as the tx and we only care to search for the form name in the rx
	ConvoScriptFiles.RemoveAll([&](const FString& String) {
		UE_LOG(LogTemp, Log, TEXT("GetConversationScript; looking at %s to see if we should remove based on convorx %s"), *String, *ConvoRx);
		// todo: need to first extract known const identifier substrings from the convorx name e.g. yvyteph from BP_YvytephConvo_Actor_C_2
		return !String.Contains(ConvoRx, ESearchCase::IgnoreCase, ESearchDir::FromStart);
	});
	for (auto Script : ConvoScriptFiles)
	{
		UE_LOG(LogTemp, Log, TEXT("GetConversationScript; filtered scripts contains %s"), *Script);
	}

	if (ConvoScriptFiles.Num() > 0)
	{
		ConvoPath.Append(ConvoScriptFiles[0]);
		if (!FFileHelper::LoadFileToString(ConvoJSON, *ConvoPath.Append(ChosenScript)))
		{
			UE_LOG(LogTemp, Error, TEXT("GetConversationScript; failed loading script %s"), *ConvoPath);
		}
		UE_LOG(LogTemp, Log, TEXT("GetConversationScript; convojson content says %s"), *ConvoJSON);
	}
	else 
	{
		UE_LOG(LogTemp, Error, TEXT("GetConversationScript; filtered script array in convopath %s size is 0"), *ConvoPath);
	}

	return  ConvoJSON;
}

UUserWidget* UConversationalComponent::StartConversation_Implementation(const FString& ConvoTx, const FString& ConvoRx, FName ClosestBone, ARyddelmystGameState* GameState, const FString& ConvoJSON)
{
	// todo: use conversationstarter to render and process convo UI, then return it.
	//  This should include an exit convo button or something at the bottom that removes the whole UI tree from the HUD.
	FString Script = ConvoJSON;
	if (Script.IsEmpty())
	{
		Script = GetConversationScript_Implementation(ConvoTx, ConvoRx, ClosestBone, GameState);
	}
	UE_LOG(LogTemp, Log, TEXT("startconversation; script says: %s"), *Script);
	return nullptr;
}

