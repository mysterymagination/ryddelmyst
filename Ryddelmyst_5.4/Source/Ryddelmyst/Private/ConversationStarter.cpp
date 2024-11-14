// Fill out your copyright notice in the Description page of Project Settings.


#include "ConversationStarter.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "LambdaButton.h"
#include "Components/PanelWidget.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "TextDisplayWidget.h"


const FString UConversationStarter::KEY_ARRAY_DIALOGUE{TEXT("dialogue")};
const FString UConversationStarter::KEY_STRING_NAME{TEXT("name")};
const FString UConversationStarter::KEY_STRING_IMAGE{TEXT("image")};
const FString UConversationStarter::KEY_ARRAY_LINES{TEXT("lines")};
const FString UConversationStarter::KEY_ARRAY_CHOICES{TEXT("choices")};
const FString UConversationStarter::KEY_STRING_TEXT{TEXT("text")};
const FString UConversationStarter::KEY_ARRAY_THOUGHTS{TEXT("thoughts")};
const FString UConversationStarter::KEY_OBJECT_DEADEND{TEXT("deadend")};
const FString UConversationStarter::KEY_OBJECT_JUMP{TEXT("jump")};
const FString UConversationStarter::KEY_STRING_CLUE{TEXT("clue")};
const FString UConversationStarter::KEY_STRING_INPUT{TEXT("input")};

const FString UConversationStarter::MATCHER_YVYTEPH_FONTOFFERTILITY{TEXT("yvyteph_fontoffertility")};
const FString UConversationStarter::MATCHER_YVYTEPH_GLORYFORM{TEXT("yvyteph_gloryform")};
const FString UConversationStarter::MATCHER_YVYTEPH_WILDFORM{TEXT("yvyteph_wildform")};
const FString UConversationStarter::MATCHER_YVYTEPH_WILDFORMSHARP{TEXT("yvyteph_wildformsharp")};
const FString UConversationStarter::MATCHER_YVYTEPH_MASTERMIND{TEXT("yvyteph_mastermind")};
const FString UConversationStarter::MATCHER_QYVNILY_WILDFLOWER{TEXT("qyvnily_wildflower")};
const FString UConversationStarter::MATCHER_QYVNILY_WILDFORM{TEXT("qyvnily_wildform")};
const FString UConversationStarter::MATCHER_QYVNILY_GLORYFORM{TEXT("qyvnily_gloryform")};
const FString UConversationStarter::MATCHER_QYVNILY_GLORYFORMRAGE{TEXT("qyvnily_gloryformrage")};

UConversationStarter::UConversationStarter()
{
    static ConstructorHelpers::FClassFinder<UUserWidget> ConvoBaseWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_ConvoBase"));
	ConvoBaseWidgetClass = ConvoBaseWidgetObj.Class;

    static ConstructorHelpers::FClassFinder<UUserWidget> PlayerDialogueWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_Convo_Dialogue_Player"));
	DialogueWidgetClass_Player = PlayerDialogueWidgetObj.Class;

    static ConstructorHelpers::FClassFinder<UUserWidget> OtherDialogueWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_Convo_Dialogue_Other"));
	DialogueWidgetClass_Other = OtherDialogueWidgetObj.Class;

    static ConstructorHelpers::FClassFinder<UUserWidget> ChoicesWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_Choices"));
	ChoicesWidgetClass = ChoicesWidgetObj.Class;
}

void UConversationStarter::Init(const FString& _ConvoTx, const FString& _ConvoRx, FName _ClosestBone, ARyddelmystGameState* _GameState)
{
    ConvoTx = _ConvoTx;
    ConvoRx = _ConvoRx;
    ClosestBone = _ClosestBone;
    GameState = _GameState;
    ScriptJsonObject = MakeShareable(new FJsonObject());
    ConvoWidget = CreateWidget<UUserWidget>(GetWorld(), ConvoBaseWidgetClass);
    ConvoContainer = Cast<UPanelWidget>(ConvoWidget->WidgetTree->FindWidget(TEXT("DialogueScrollBox")));
}

FString UConversationStarter::GetScript()
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
		FString ScriptName = CalculateScriptName(CharacterName, GameState);
		ConvoPath.Append(ScriptName);
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

FString UConversationStarter::CalculateScriptName(const FString& CharacterName, ARyddelmystGameState* GameState)
{
	FString ConvoScriptName;
	if (CharacterName.Equals(MATCHER_YVYTEPH_MASTERMIND))
	{
		if (GameState->ClueState == ARyddelmystGameState::STATE_CLUE_INPUT_CRUELTY_QUERY)
		{
			if (
				GameState->UserInputValue.Contains(ARyddelmystGameState::VALUE_INPUT_EGG_TOKEN) && 
				GameState->UserInputValue.Contains(ARyddelmystGameState::VALUE_INPUT_LAVA_TOKEN)
			)
			{
				ConvoScriptName = TEXT("Undercarriage_Desire_Mastery_Yvyteph_Mastermind.json");
			}
			else 
			{
				ConvoScriptName = TEXT("Undercarriage_Rejected_Angel_Yvyteph_Mastermind.json");
			}
		}
	}
	else if (CharacterName.Equals(MATCHER_YVYTEPH_FONTOFFERTILITY))
	{
		ConvoScriptName = TEXT("Intro_Yvyteph_FontOfFertility.json");
	}
	else if (CharacterName.Equals(MATCHER_QYVNILY_WILDFLOWER))
	{
		ConvoScriptName = TEXT("Intro_Qyvnily_WildFlower.json");
	}
	// todo: other characters
	//return ConvoScriptName;

    return TEXT("TestJSON.json");
}

FString UConversationStarter::MatchCharacter(const FString& ActorName)
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

UUserWidget* UConversationStarter::GenerateConversationUI(const FString& Script)
{
    // todo: parse script json into UI elements added to a wrapper slate widget; for simplicity and prettyness I guess the best approach would be to create a UI asset in the editor that acts as a scrollable container and then add generated elements such as images, text, and buttons from the parsing.
    ///UUserWidget* ConvoWidget = CreateWidget<UUserWidget>(GetWorld(), ConvoBaseWidgetClass);
    ///UScrollBox* ScrollBox = Cast<UScrollBox>(ConvoWidget->WidgetTree->FindWidget(TEXT("DialogueScrollBox")));
    ParseConversationScript(Script);
    if (ScriptJsonObject)///(CurrentScriptJsonObject)
    {
        auto DialogueElementsArray = ScriptJsonObject->GetArrayField(KEY_ARRAY_DIALOGUE);///CurrentScriptJsonObject->GetArrayField(KEY_ARRAY_DIALOGUE);
        UE_LOG(LogTemp, Warning, TEXT("ParseConvoScript; dialogue array has %d elements"), DialogueElementsArray.Num());

        /*
        // todo: set current dialogue json object in a desperate bid to keep the damn nested crap alive through stack pops
        CurrentDialogueJsonObject = MakeShareable(new FJsonObject());
        CurrentDialogueJsonObject->SetArrayField(KEY_ARRAY_DIALOGUE, std::move(DialogueElementsArray));
        */

        ParseDialogue(ConvoWidget, ConvoContainer, DialogueElementsArray);
    }
    else 
    {
        UE_LOG(LogTemp, Error, TEXT("GenerateConvoUI; failed to extract top level script JSON object"));
    }
    return ConvoWidget;
}

void UConversationStarter::ParseConversationScript(const FString& Script)
{
    ///{
        auto Reader = TJsonReaderFactory<>::Create(Script);
        if (FJsonSerializer::Deserialize(Reader, ScriptJsonObject))
        {
            UE_LOG(LogTemp, Warning, TEXT("ParseConvoScript; deserialized json"));
            ///CurrentScriptJsonObject = ScriptJsonObject;

            /*
            auto DialogueElementsArrayExperiment = CurrentScriptJsonObject->GetArrayField(KEY_ARRAY_DIALOGUE);
            UE_LOG(LogTemp, Warning, TEXT("ParseConvoScript; in scope of deserialize, current script object has dialogue array of %d elements"), DialogueElementsArrayExperiment.Num());
            */
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("ParseConvoScript; deserializing json failed"));
        }
    ///}
    /* this out of immediate scope business all appears to be fine. *shurg*
    auto DialogueElementsArrayExperiment = CurrentScriptJsonObject->GetArrayField(KEY_ARRAY_DIALOGUE);
    UE_LOG(LogTemp, Warning, TEXT("ParseConvoScript; out scope of deserialize, current script object has dialogue array of %d elements"), DialogueElementsArrayExperiment.Num());
    
    const TSharedPtr<FJsonObject>* DialogueObject;
    if (DialogueElementsArrayExperiment[0]->TryGetObject(DialogueObject))
    {
    UE_LOG(LogTemp, Warning, TEXT("ParseConvoScript; out scope of deserialize, current script object's dialogue array first element has image named %s"), *(*DialogueObject)->GetStringField(KEY_STRING_IMAGE));
    }

    CurrentDialogueJsonObject = MakeShareable(new FJsonObject());
    CurrentDialogueJsonObject->SetArrayField(KEY_ARRAY_DIALOGUE, DialogueElementsArrayExperiment);
    auto DialogueElementsArrayStat = CurrentDialogueJsonObject->GetArrayField(KEY_ARRAY_DIALOGUE);
    UE_LOG(LogTemp, Warning, TEXT("ParseConvoScript; out scope of deserialize, current dialogue object has dialogue array of %d elements"), DialogueElementsArrayStat.Num());
    const TSharedPtr<FJsonObject>* DialogueObject2;
    if (DialogueElementsArrayStat[0]->TryGetObject(DialogueObject2))
    {
        UE_LOG(LogTemp, Warning, TEXT("ParseConvoScript; out scope of deserialize, current dialogue object's dialogue array first element has image named %s"), *(*DialogueObject2)->GetStringField(KEY_STRING_IMAGE));
    }
    */
}

void UConversationStarter::ParseDialogue(UUserWidget* ConvoWidgetDontUse, UPanelWidget* ContainerDontUse, const TArray<TSharedPtr<FJsonValue>>& DialogueElementsArray)
{
    // todo: even storing these fellas in a dedicated sharedptr leads to them coming up as 0 length array once we call into the function afresh. It seems that the inflated JSON only lives in memory within the call scope of deserialize()? But there must be some way to hang onto an entire inflated JSON tree...
    ///auto DialogueElementsArrayExperiment = ScriptJsonObject->GetArrayField(KEY_ARRAY_DIALOGUE);///CurrentDialogueJsonObject->GetArrayField(KEY_ARRAY_DIALOGUE);
    ///UE_LOG(LogTemp, Warning, TEXT("ParseDialogue; current dialogue object has dialogue array of %d elements"), DialogueElementsArrayExperiment.Num());
    for (auto DialogueElement : DialogueElementsArray)
    {
        UE_LOG(LogTemp, Warning, TEXT("ParseDialogue; got dialogue element in dialogues array"));

        const TSharedPtr<FJsonObject>* DialogueObject;
        if (DialogueElement->TryGetObject(DialogueObject))
        {
            UE_LOG(LogTemp, Warning, TEXT("ParseDialogue; got dialogue object"));
            FString PortraitName = (*DialogueObject)->GetStringField(KEY_STRING_IMAGE);
            UTextDisplayWidget* DialogueWidget{nullptr};
            // todo: my json schema doesn't have a cleaner way to represent point of view atm, so just grep for maya in the portrait name *sigh*
            UE_LOG(LogTemp, Warning, TEXT("ParseDialogue; portrait matching"));
            if (PortraitName.Contains(TEXT("maya"), ESearchCase::IgnoreCase, ESearchDir::FromStart))
            {
                DialogueWidget = Cast<UTextDisplayWidget>(ConvoWidget->WidgetTree->ConstructWidget(DialogueWidgetClass_Player));
            }
            else 
            {
                DialogueWidget = Cast<UTextDisplayWidget>(ConvoWidget->WidgetTree->ConstructWidget(DialogueWidgetClass_Other));
            }
            UE_LOG(LogTemp, Warning, TEXT("ParseDialogue; looking at lines"));
            const TArray<TSharedPtr<FJsonValue>>& LinesArray = (*DialogueObject)->GetArrayField(KEY_ARRAY_LINES);
            FString LinesAggregate;
            for (auto Line : LinesArray)
            {
                LinesAggregate.Append(Line->AsString());
            }
            // todo: create a thoughts dialogue widget template in editor with a cheeky thought baloon border or something and italic text and load the thought text into that widget rather than cramming everything into the dialogue widget.
            UE_LOG(LogTemp, Warning, TEXT("ParseDialogue; looking at thoughts"));
            const TArray<TSharedPtr<FJsonValue>>* ThoughtsArray;
            if ((*DialogueObject)->TryGetArrayField(KEY_ARRAY_THOUGHTS, ThoughtsArray))
            {

                for (auto Thought : *ThoughtsArray)
                {
                    LinesAggregate.Append(Thought->AsString());
                }
            }
            UE_LOG(LogTemp, Warning, TEXT("ParseDialogue; setting text"));
            DialogueWidget->SetText(FText::FromString(LinesAggregate));
            FString PortraitPath = FString::Printf(TEXT("/Game/Ryddelmyst_Assets/Sprites/%s_Portrait_Sprite.%s_Portrait_Sprite"), *PortraitName, *PortraitName);
            UE_LOG(LogTemp, Warning, TEXT("ParseConvoScript; portraitpath is %s"), *PortraitPath);
            UPaperSprite* Portrait = LoadObject<UPaperSprite>(nullptr, *PortraitPath);
            DialogueWidget->SetPortrait(Portrait);
            ConvoContainer->AddChild(DialogueWidget);
            // todo: when pulling in strings from a lines or text data, use NSLOCTEXT and create it using format text;
            //  This will entail having variables in the text like FText::FormatNamed(LOCTEXT("SnippetHeader", "There are {Count} snippets in group {Name}"),TEXT("Count"), SnippetCount, TEXT("Name"), GroupNameText); which
            //  also means we'll need to manually parse out those variables from the JSON and use reflection or something
            //  to look up the appropriate GameState symbol... or just use the existing jank variable replacement I wrote 
            //  for LibraryBookWidget. Or nevermind variable templates in convo text for now.
            UE_LOG(LogTemp, Warning, TEXT("ParseDialogue; choicing it up"));
            const TArray<TSharedPtr<FJsonValue>>* ChoicesArray;
            if ((*DialogueObject)->TryGetArrayField(KEY_ARRAY_CHOICES, ChoicesArray))
            {
                UUserWidget* ChoicesWidget = ConvoWidget->WidgetTree->ConstructWidget(ChoicesWidgetClass);
                ConvoContainer->AddChild(ChoicesWidget);
                auto* ChoicesList = Cast<UScrollBox>(ChoicesWidget->WidgetTree->FindWidget(TEXT("ScrollBox_Choices")));
                UE_LOG(LogTemp, Error, TEXT("ParseConvoScript; choiceslist says %p"), ChoicesList);
                
                // todo: remove choiceswidget after a choice is made?
                // populate choiceswidget with buttons hosting the choices array text
                for (auto Choice : *ChoicesArray)
                {
                    const TSharedPtr<FJsonObject>* ChoiceObject;
                    if (Choice->TryGetObject(ChoiceObject))
                    {
                        auto* ChoiceButton = ChoicesWidget->WidgetTree->ConstructWidget<ULambdaButton>();
                        ChoicesList->AddChild(ChoiceButton);
                        auto* ChoiceTextWidget = ChoicesWidget->WidgetTree->ConstructWidget<UTextBlock>();
                        FString ChoiceText = (*ChoiceObject)->GetStringField(KEY_STRING_TEXT);
                        ChoiceTextWidget->SetText(FText::FromString(ChoiceText));
                        ChoiceTextWidget->SetColorAndOpacity(FSlateColor(FLinearColor(0.f, 0.f, 0.f, 1.f)));
                        ChoiceButton->AddChild(ChoiceTextWidget);
                        const TArray<TSharedPtr<FJsonValue>>* SubDialogueElements;
                        const TSharedPtr<FJsonObject>* LeafNode;
						if (Choice->AsObject()->TryGetArrayField(KEY_ARRAY_DIALOGUE, SubDialogueElements))
						{
                            // ParseDialogue(ConvoWidget, Container, *SubDialogueElements); // checking to make sure dumping everything recursively works

                            // todo: set current dialogue json object in a desperate bid to keep the damn nested crap alive through stack pops
                            ///CurrentDialogueJsonObject = MakeShareable(new FJsonObject());
                            ///CurrentDialogueJsonObject->SetArrayField(KEY_ARRAY_DIALOGUE, std::move(*SubDialogueElements));
                            /* todo: so far I've assumed the pop off the stack from lambda install to lambda exec is the problem; check what happens if we call ParseDialogue() recursively. EDIT: yeah the recursive approach works, so I guess the above issue is the problem. But why? I hang on to the root jsonobject in a class member field so none of it should be getting deallocated. Another experiment might be to try calling ParseDialogue multiple times from some other context where the initial deserialized jsonobject will have gone out of scope and see if we hit the same problem, just to see if its about the initial sharedptr going out of scope or maybe something else?
                            */

							// install subdialogue elements to OnClick lambda event
                            ///auto DialogueElementsArrayStat = CurrentDialogueJsonObject->GetArrayField(KEY_ARRAY_DIALOGUE);
                            ///UE_LOG(LogTemp, Warning, TEXT("ParseDialogue; just before setting up subdialogue lambda, current script says %p and current dialogue says %p. Current dialogue has %d elements."), CurrentScriptJsonObject.Get(), CurrentDialogueJsonObject.Get(), DialogueElementsArrayStat.Num());

							ChoiceButton->LambdaEvent.BindLambda([&]() 
							{
                                ///auto DialogueElementsArrayStat = CurrentDialogueJsonObject->GetArrayField(KEY_ARRAY_DIALOGUE);
                                ///UE_LOG(LogTemp, Warning, TEXT("ParseDialogue; in subdialogue lambda, current script says %p and current dialogue says %p. Current dialogue has %d elements."), CurrentScriptJsonObject.Get(), CurrentDialogueJsonObject.Get(), DialogueElementsArrayStat.Num());
                                // todo: getting a SIGBUS or SEGFAULT when this call tries to unpack the SubDialogueElements; not clear why. I thought maybe I needed to hang on to the sharedptr we get in deserialize, but that didn't help. I don't know when/how they 'inflate' JSON string elements into actual JsonObjects in memory though; I assumed it would process the whole JSON file in deserialize, but maybe not? Maybe the subdialogue json objects are only allocated memory when I call TryGetArrayField() and they don't remain valid after we pop off the stack waiting for the button press event to call this lambda? Regardless, we could always just change the API to expect a sharedptr to jsonobject and then create said object to host the subdialogue and capture it here... but then that guy would also go out of scope between lambda install and lambda execute. COuld always hang on to another field for the current dialogue subtree, I guess. *sighs, quietly mutilates API* ... or not since other choices with subdialogues would overwrite it. hrmn.
								ParseDialogue(ConvoWidget, ConvoContainer, *SubDialogueElements);
							});
						}
                        // todo: oops, some crossed wires here -- the choice object itself does not have jump and deadend leafnodes; these currently live on the dialogue object itself. Since those should never be processed until the player has the chance to read the rest of the dialogue subtree, it might make sense to move these into the choice object. However, there are some cases where there isn't actually a `choice` per se e.g. Yvyteph throws the player out of the conversation, so it could be confusing to script it that way. Plus it might be nice to have a click-to-continue mechanism between dialogue elements anyway, at least maybe at some transition points (which leaf nodes could be an example of). So yeah I kinda like the notion of moving these leaf node processings out into the dialogue element as part of a collection of transition attributes that need user interaction; choices itself could be one, jump and deadend could be ones, and maybe something that just says 'wait' or something to indicate a button with 'continue...' should be rendered. Maybe this transition should be its own object, maybe with a type value so we don't have to lean on unintuitively mutex if chains? 
                        /*
                        else if (Choice->AsObject()->TryGetObjectField(KEY_OBJECT_JUMP, LeafNode))
                        {
                            FString Clue;
                            if ((*LeafNode)->TryGetStringField(KEY_STRING_CLUE, Clue))
                            {
                                ChoiceButton->LambdaEvent.BindLambda([&]() 
                                {
                                    GameState->ClueState = Clue;
                                    // re-run script selection logic  
                                    ParseConversationScript(GetScript());
                                    if (CurrentScriptJsonObject)
                                    {
                                        auto SubDialogueElementsArray = CurrentScriptJsonObject->GetArrayField(KEY_ARRAY_DIALOGUE);
                                        UE_LOG(LogTemp, Warning, TEXT("ParseDialogue; subdialogue array has %d elements"), SubDialogueElementsArray.Num());
                                        ParseDialogue(ConvoWidget, Container, SubDialogueElementsArray);
                                    }
                                    else
                                    {
                                        UE_LOG(LogTemp, Error, TEXT("ParseDialogue; current script object is null"));
                                    }
                                });
                            }
                            else
                            {
                                UE_LOG(LogTemp, Error, TEXT("ParseDialogue; we do not have a clue for jump in dialogue element %s"), *(*DialogueObject)->GetStringField(KEY_STRING_NAME));
                            }
                        }
                        else if (Choice->AsObject()->TryGetObjectField(KEY_OBJECT_DEADEND, LeafNode))
                        {
                            FString Clue;
                            if ((*LeafNode)->TryGetStringField(KEY_STRING_CLUE, Clue))
                            {
                                GameState->ClueState = Clue;
                                // todo: exit conversation
                            }
                            else
                            {
                                UE_LOG(LogTemp, Error, TEXT("ParseDialogue; we do not have a clue for deadend in dialogue element %s"), *(*DialogueObject)->GetStringField(KEY_STRING_NAME));
                            }
                        }
                        */
                        // install OnClicked behavior, instructing it to simply exec the lambda
						ChoiceButton->OnClicked.AddDynamic(ChoiceButton, &ULambdaButton::ExecLambda);
						// todo: install onclick based on presence of nested dialogue trees, jumps, deadends etc.
                        //  There's essentially three cases we need to process:
                        //  1. dialogue: for this key, we'll want to call ParseDialogue() 'recursively' (won't technically be recursive since there's a button press UI event in the way) and pass in the dialogue JSON array for parsing.
                        //  2. jump: when we see this key we should call out to the script selection logic anew, where the GameState clue value will inform what script should be selected. Clue state should be cleared after this?
                        //  3. deadend: similar to jump behavior, except instead of going into script selection we want to exit the conversation and do something in the world determined by the GameState clue value. TBD how best to do that from here since it implies hitting up the HUD convo exit fn... I guess we could always get cheeky and FindWidget() the convo exit button in the ConvoWidget and press it, then have the convo exit fn check GameState for a lingering clue? A FSM with states for convo and gameplay etc. could be handy, with a transition from convo -> game doing a check of clue for this purpose would be ideal. Dunno if this one case is enough to warrant a whole FSM, but I'm gonna leave this here anyway. Clue state should be cleared after this?
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("ParseConvoScript; choice jsonval could not be converted to object"));
                    }
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("ParseConvoScript; no choices found"));
            }
            // todo: text input prompt processing
        }
    }
}