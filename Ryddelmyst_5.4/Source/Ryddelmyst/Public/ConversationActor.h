// Copyright Jeff Creswell 2024. Subject to the GPL v3.0 license.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ITalkable.h"
#include "ConversationActor.generated.h"

UCLASS()
class RYDDELMYST_API AConversationActor : public AActor, public ITalkable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AConversationActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	FString GetConversationScript_Implementation(const FString& ConvoTx, const FString& ConvoRx, FName ClosestBone, ARyddelmystGameState* GameState);

};
