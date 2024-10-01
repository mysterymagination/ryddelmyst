// Copyright Jeff Creswell 2024. Subject to the GPL v3.0 license.


#include "ConversationActor.h"

// Sets default values
AConversationActor::AConversationActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AConversationActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AConversationActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FString AConversationActor::GetConversationScript_Implementation(FString ConvoTx, FString ConvoRx, FName ClosestBone, ARyddelmystGameState* GameState)
{
	// todo: use params to figure out the conversation script we should use.
	// todo: should each convo actor instance override this with a limited set of relevant scripts that query game state only?
	//  Maybe it would be a better idea to have each convo actor instance have an array of scripts and then choose from among them based on game state? I'm
	//  just not a big fan of the notion of having a global convo 'db' which every convo actor queries from scratch; I know we don't care much for optimization for the demo,
	//  but it's not very data driven of me -- consider the case where we want to mod a convo later: presently that could involve modifying both a JSON script
	//  AND this shared monolith of logic which could cause merry hell among the convo actor instances. If instead we have the data provided by the instance itself
	//  we need only mod one instance (or convo actor subclass) and possibly its particular state query logic. Subtle difference at this scale, but non-data-drivenness in game design makes my spirit itch. 
}

