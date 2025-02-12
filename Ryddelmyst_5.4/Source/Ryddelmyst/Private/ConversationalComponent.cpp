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

UUserWidget* UConversationalComponent::StartConversation_Implementation(const FString& ConvoTx, const FString& ConvoRx, FName ClosestBone, ARyddelmystGameState* GameState, const FString& ConvoJSON)
{
	// use conversationstarter to render and process convo UI, then return it.
	UE_LOG(LogTemp, Log, TEXT("startconversation; convotx: %s, convorx %s"), *ConvoTx, *ConvoRx);
	ConversationStarter->Init(ConvoTx, ConvoRx, ClosestBone, GameState);
	FString Script = ConvoJSON;
	if (Script.IsEmpty())
	{
		Script = ConversationStarter->GetScript();
	}
	UE_LOG(LogTemp, Log, TEXT("startconversation; script says: %s"), *Script);
	return ConversationStarter->GenerateConversationUI(Script);
}

