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
	TArray<FString>& ConvoScriptFiles;
	// enumerate list of scripts
	FileManager.FindFiles(ConvoScriptFiles, *ConvoPath, true, false);
	// filter list by convo tx and rx, particularly rx goddess form
	// at the moment, we only ever have Maya as the tx and we only care to search for the form name in the rx
	ConvoScriptFiles.RemoveAll([](const FString& String) {
		return !String.Contains(ConvoRx);
	});
	for (auto Script : ConvoScriptFiles)
	{
		UE_LOG(LogTemp, Log, TEXT("GetConversationScript; filtered scripts contains %s"), *Script);
	}
	if (!FFileHelper::LoadFileToString(ConvoJSON, *ConvoPath.Append(ChosenScript)))
	{
		UE_LOG(LogTemp, Error, TEXT("GetConversationScript; failed loading script %s"), *ConvoPath);
	}
	return  ConvoJSON;
}

void UConversationalComponent::StartConversation_Implementation(const FString& ConvoTx, const FString& ConvoRx, FName ClosestBone, ARyddelmystGameState* GameState, const FString& ConvoJSON)
{
	// todo: use conversationstarter to render and process convo UI.
	FString Script = GetConversationScript_Implementation(ConvoTx, ConvoRx, ClosestBone, GameState);
}

