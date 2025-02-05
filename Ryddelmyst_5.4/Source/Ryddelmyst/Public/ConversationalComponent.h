// Copyright Jeff Creswell 2024. Subject to the GPL v3.0 license.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ITalkable.h"
#include "ConversationStarter.h"
#include "RyddelmystGameState.h"
#include "ConversationalComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RYDDELMYST_API UConversationalComponent : public UActorComponent, public ITalkable
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UConversationStarter* ConversationStarter;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FString ConversationalName;

public:	
	// Sets default values for this component's properties
	UConversationalComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UUserWidget* StartConversation_Implementation(const FString& ConvoTx, const FString& ConvoRx, FName ClosestBone, ARyddelmystGameState* GameState, const FString& ConvoJSON);

	FString GetConversationalName_Implementation() {return ConversationalName; };
};

