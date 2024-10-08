// Copyright Jeff Creswell 2024. Subject to the GPL v3.0 license.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ITalkable.h"
#include "ConversationStarter.h"
#include "ConversationalComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RYDDELMYST_API UConversationalComponent : public UActorComponent, public ITalkable
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UConversationStarter* ConversationStarter;

public:
	static const FString MATCHER_YVYTEPH;
	static const FString MATCHER_QYVNILY;

public:	
	// Sets default values for this component's properties
	UConversationalComponent();
private:
	/**
	 * @brief Tries to match a known game character name token within a given actor's full GetName() result.
	 * @param ActorName - GetName() result for an actor in a conversation.
	 * @return matched character name or empty string. 
	 */
	UFUNCTION()
	FString MatchCharacter(const FString& ActorName);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FString GetConversationScript_Implementation(const FString& ConvoTx, const FString& ConvoRx, FName ClosestBone, ARyddelmystGameState* GameState);
	UUserWidget* StartConversation_Implementation(const FString& ConvoTx, const FString& ConvoRx, FName ClosestBone, ARyddelmystGameState* GameState, const FString& ConvoJSON);
};

