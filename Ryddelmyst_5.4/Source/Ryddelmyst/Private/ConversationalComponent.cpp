// Copyright Jeff Creswell 2024. Subject to the GPL v3.0 license.


#include "ConversationalComponent.h"

const FString UConversationalComponent::MATCHER_YVYTEPH_FONTOFFERTILITY{TEXT("yvyteph_fontoffertility")};
const FString UConversationalComponent::MATCHER_YVYTEPH_GLORYFORM{TEXT("yvyteph_gloryform")};
const FString UConversationalComponent::MATCHER_YVYTEPH_WILDFORM{TEXT("yvyteph_wildform")};
const FString UConversationalComponent::MATCHER_YVYTEPH_WILDFORMSHARP{TEXT("yvyteph_wildformsharp")};
const FString UConversationalComponent::MATCHER_YVYTEPH_MASTERMIND{TEXT("yvyteph_mastermind")};
const FString UConversationalComponent::MATCHER_QYVNILY_WILDFLOWER{TEXT("qyvnily_wildflower")};
const FString UConversationalComponent::MATCHER_QYVNILY_WILDFORM{TEXT("qyvnily_wildform")};
const FString UConversationalComponent::MATCHER_QYVNILY_GLORYFORM{TEXT("qyvnily_gloryform")};
const FString UConversationalComponent::MATCHER_QYVNILY_GLORYFORMRAGE{TEXT("qyvnily_gloryformrage")};

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

FString UConversationalComponent::MatchCharacter(const FString& ActorName)
{
	// sequence matters because some forms have subforms like gloryform -> gloryformrage, so we want to make sure
	// we search from the bottom up so to speak in terms of branch terminals to avoid partial matches. Some sort of 
	// more robust naming scheme or parsing logic to account for this possibility might be a better solution,
	// but I couldn't be arsed. 
	if (ActorName.Contains(MATCHER_YVYTEPH_FONTOFFERTILITY, ESearchCase::IgnoreCase, ESearchDir::FromStart))
	{
		return MATCHER_YVYTEPH_FONTOFFERTILITY;
	}
	else if (ActorName.Contains(MATCHER_YVYTEPH_GLORYFORM, ESearchCase::IgnoreCase, ESearchDir::FromStart))
	{
		return MATCHER_YVYTEPH_GLORYFORM;
	}
	else if (ActorName.Contains(MATCHER_YVYTEPH_WILDFORMSHARP, ESearchCase::IgnoreCase, ESearchDir::FromStart))
	{
		return MATCHER_YVYTEPH_WILDFORMSHARP;
	}
	else if (ActorName.Contains(MATCHER_YVYTEPH_WILDFORM, ESearchCase::IgnoreCase, ESearchDir::FromStart))
	{
		return MATCHER_YVYTEPH_WILDFORM;
	}
	else if (ActorName.Contains(MATCHER_YVYTEPH_MASTERMIND, ESearchCase::IgnoreCase, ESearchDir::FromStart))
	{
		return MATCHER_YVYTEPH_MASTERMIND;
	}
	else if (ActorName.Contains(MATCHER_QYVNILY_WILDFLOWER, ESearchCase::IgnoreCase, ESearchDir::FromStart))
	{
		return MATCHER_QYVNILY_WILDFLOWER;
	}
	else if (ActorName.Contains(MATCHER_QYVNILY_WILDFORM, ESearchCase::IgnoreCase, ESearchDir::FromStart))
	{
		return MATCHER_QYVNILY_WILDFORM;
	}
	else if (ActorName.Contains(MATCHER_QYVNILY_GLORYFORMRAGE, ESearchCase::IgnoreCase, ESearchDir::FromStart))
	{
		return MATCHER_QYVNILY_GLORYFORMRAGE;
	}
	else if (ActorName.Contains(MATCHER_QYVNILY_GLORYFORM, ESearchCase::IgnoreCase, ESearchDir::FromStart))
	{
		return MATCHER_QYVNILY_GLORYFORM;
	}
	else 
	{
		return TEXT("");
	}
}

FString UConversationalComponent::GetConversationScript_Implementation(const FString& ConvoTx, const FString& ConvoRx, FName ClosestBone, ARyddelmystGameState* GameState)
{
	FString ConvoPath = FPaths::ProjectContentDir().Append(TEXT("Ryddelmyst_Assets/Text/Dialogue/"));
	FString ChosenScript;
	FString ConvoJSON;
	IFileManager& FileManager = IFileManager::Get();
	TArray<FString> ConvoScriptFiles;
	// enumerate list of scripts
	FileManager.FindFiles(ConvoScriptFiles, *ConvoPath, TEXT("*.json"));
	// filter list by convo tx and rx, particularly rx goddess form
	// at the moment, we only ever have Maya as the tx and we only care to search for the form name in the rx
	UE_LOG(LogTemp, Log, TEXT("GetConversationScript; convorx says %s"), *ConvoRx);
	FString CharacterName = MatchCharacter(ConvoRx);
	if (!CharacterName.IsEmpty())
	{
		ConvoScriptFiles.RemoveAll([&](const FString& String) {
			UE_LOG(LogTemp, Log, TEXT("GetConversationScript; looking at %s to see if we should remove based on convorx %s"), *String, *CharacterName);
			return !String.Contains(CharacterName, ESearchCase::IgnoreCase, ESearchDir::FromStart);
		});
		for (auto Script : ConvoScriptFiles)
		{
			UE_LOG(LogTemp, Log, TEXT("GetConversationScript; filtered scripts contains %s"), *Script);
		}
	}
	else 
	{
		UE_LOG(LogTemp, Error, TEXT("GetConversationScript; convorx %s did not match any character names"), *ConvoRx);
	}

	if (ConvoScriptFiles.Num() > 0)
	{
		// todo: use game state to find best fit script
		//  In particular, look at states related to Yvyteph 
		//  Mastermind stuff since she has several scripts that select from 
		//  and possibly are jumping to from another script. For the jal, maybe the clue
		//  could be shoved into the gamestate and then we can happily call this fella
		//  recursively from ConversationStarter's processing?
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

