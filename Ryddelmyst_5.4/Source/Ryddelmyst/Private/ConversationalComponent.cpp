// Copyright Jeff Creswell 2024. Subject to the GPL v3.0 license.


#include "ConversationalComponent.h"
#include "RyddelmystGameState.h"

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
	return ConversationStarter->GetScript(ConvoTx, ConvoRx, ClosestBone, GameState);
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
	return ConversationStarter->ParseConversationScript(Script, GameState);
}

