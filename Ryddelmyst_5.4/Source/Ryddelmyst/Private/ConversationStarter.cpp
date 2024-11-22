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
#include "Kismet/GameplayStatics.h"
#include "RyddelmystHUD.h"

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
const FString UConversationStarter::KEY_OBJECT_TRANSITION{TEXT("transition")};
const FString UConversationStarter::KEY_STRING_TYPE{TEXT("type")};

const FString UConversationStarter::VALUE_TRANSITION_TYPE_JUMP{TEXT("jump")};
const FString UConversationStarter::VALUE_TRANSITION_TYPE_DEADEND{TEXT("deadend")};

const FString UConversationStarter::MATCHER_YVYTEPH_FONTOFFERTILITY{TEXT("yvyteph_fontoffertility")};
const FString UConversationStarter::MATCHER_YVYTEPH_GLORYFORM{TEXT("yvyteph_gloryform")};
const FString UConversationStarter::MATCHER_YVYTEPH_WILDFORM{TEXT("yvyteph_wildform")};
const FString UConversationStarter::MATCHER_YVYTEPH_WILDFORMSHARP{TEXT("yvyteph_wildformsharp")};
const FString UConversationStarter::MATCHER_YVYTEPH_MASTERMIND{TEXT("yvyteph_mastermind")};
const FString UConversationStarter::MATCHER_QYVNILY_WILDFLOWER{TEXT("qyvnily_wildflower")};
const FString UConversationStarter::MATCHER_QYVNILY_WILDFORM{TEXT("qyvnily_wildform")};
const FString UConversationStarter::MATCHER_QYVNILY_GLORYFORM{TEXT("qyvnily_gloryform")};
const FString UConversationStarter::MATCHER_QYVNILY_GLORYFORMRAGE{TEXT("qyvnily_gloryformrage")};
const FString UConversationStarter::MATCHER_TEST{TEXT("test")};

UConversationStarter::UConversationStarter()
{
    static ConstructorHelpers::FClassFinder<UUserWidget> ConvoBaseWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_ConvoBase"));
	ConvoBaseWidgetClass = ConvoBaseWidgetObj.Class;

    static ConstructorHelpers::FClassFinder<UUserWidget> PlayerDialogueWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_Convo_Dialogue_Player"));
	DialogueWidgetClass_Player = PlayerDialogueWidgetObj.Class;

    static ConstructorHelpers::FClassFinder<UUserWidget> OtherDialogueWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_Convo_Dialogue_Other"));
	DialogueWidgetClass_Other = OtherDialogueWidgetObj.Class;

    static ConstructorHelpers::FClassFinder<UUserWidget> PlayerThoughtsWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_Convo_Dialogue_Thoughts_Player"));
	ThoughtsWidgetClass_Player = PlayerThoughtsWidgetObj.Class;

    static ConstructorHelpers::FClassFinder<UUserWidget> OtherThoughtsWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_Convo_Dialogue_Thoughts_Other"));
	ThoughtsWidgetClass_Other = OtherThoughtsWidgetObj.Class;

    static ConstructorHelpers::FClassFinder<UUserWidget> ChoicesWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_Choices"));
	ChoicesWidgetClass = ChoicesWidgetObj.Class;

    static ConstructorHelpers::FClassFinder<UUserWidget> DividerWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_TextDivide"));
	DividerWidgetClass = DividerWidgetObj.Class;
}

void UConversationStarter::Init(const FString& _ConvoTx, const FString& _ConvoRx, FName _ClosestBone, ARyddelmystGameState* _GameState)
{
    ConvoTx = _ConvoTx;
    ConvoRx = _ConvoRx;
    ClosestBone = _ClosestBone;
    GameState = _GameState;
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
		FString ScriptName = CalculateScriptName(CharacterName);
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

FString UConversationStarter::CalculateScriptName(const FString& CharacterName)
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
    else if (CharacterName.Equals(MATCHER_TEST))
	{
		ConvoScriptName = TEXT("TestJSON.json");
	}
	// todo: other characters
	return ConvoScriptName;
}

FString UConversationStarter::MatchCharacter(const FString& ActorName)
{
    FString CharacterName = TEXT("");
	// sequence matters because some forms have subforms like gloryform -> gloryformrage, so we want to make sure
	// we search from the bottom up so to speak in terms of branch terminals to avoid partial matches. Some sort of 
	// more robust naming scheme or parsing logic to account for this possibility might be a better solution,
	// but I couldn't be arsed. 
	if (ActorName.Contains(MATCHER_YVYTEPH_FONTOFFERTILITY, ESearchCase::IgnoreCase, ESearchDir::FromStart))
	{
		CharacterName = MATCHER_YVYTEPH_FONTOFFERTILITY;
	}
	else if (ActorName.Contains(MATCHER_YVYTEPH_GLORYFORM, ESearchCase::IgnoreCase, ESearchDir::FromStart))
	{
		CharacterName = MATCHER_YVYTEPH_GLORYFORM;
	}
	else if (ActorName.Contains(MATCHER_YVYTEPH_WILDFORMSHARP, ESearchCase::IgnoreCase, ESearchDir::FromStart))
	{
		CharacterName = MATCHER_YVYTEPH_WILDFORMSHARP;
	}
	else if (ActorName.Contains(MATCHER_YVYTEPH_WILDFORM, ESearchCase::IgnoreCase, ESearchDir::FromStart))
	{
		CharacterName = MATCHER_YVYTEPH_WILDFORM;
	}
	else if (ActorName.Contains(MATCHER_YVYTEPH_MASTERMIND, ESearchCase::IgnoreCase, ESearchDir::FromStart))
	{
		CharacterName = MATCHER_YVYTEPH_MASTERMIND;
	}
	else if (ActorName.Contains(MATCHER_QYVNILY_WILDFLOWER, ESearchCase::IgnoreCase, ESearchDir::FromStart))
	{
		CharacterName = MATCHER_QYVNILY_WILDFLOWER;
	}
	else if (ActorName.Contains(MATCHER_QYVNILY_WILDFORM, ESearchCase::IgnoreCase, ESearchDir::FromStart))
	{
		CharacterName = MATCHER_QYVNILY_WILDFORM;
	}
	else if (ActorName.Contains(MATCHER_QYVNILY_GLORYFORMRAGE, ESearchCase::IgnoreCase, ESearchDir::FromStart))
	{
		CharacterName = MATCHER_QYVNILY_GLORYFORMRAGE;
	}
	else if (ActorName.Contains(MATCHER_QYVNILY_GLORYFORM, ESearchCase::IgnoreCase, ESearchDir::FromStart))
	{
		CharacterName = MATCHER_QYVNILY_GLORYFORM;
	}
	
    // override with test script if actor name suggests it
    if (ActorName.Contains(MATCHER_TEST, ESearchCase::IgnoreCase, ESearchDir::FromStart))
	{
		CharacterName = MATCHER_TEST;
	}

    return CharacterName;
}

UUserWidget* UConversationStarter::GenerateConversationUI(const FString& Script)
{
    ConvoWidget = CreateWidget<UUserWidget>(GetWorld(), ConvoBaseWidgetClass);
    ConvoContainer = Cast<UScrollBox>(ConvoWidget->WidgetTree->FindWidget(TEXT("DialogueScrollBox")));
    ParseConversationScript(Script);
    if (ScriptJsonObject)
    {
        ParseDialogue(ScriptJsonObject);
    }
    else 
    {
        UE_LOG(LogTemp, Error, TEXT("GenerateConvoUI; failed to extract top level script JSON object"));
    }
    return ConvoWidget;
}

void UConversationStarter::ParseConversationScript(const FString& Script)
{
    ScriptJsonObject = MakeShareable(new FJsonObject());
    auto Reader = TJsonReaderFactory<>::Create(Script);
    if (FJsonSerializer::Deserialize(Reader, ScriptJsonObject))
    {
        UE_LOG(LogTemp, Warning, TEXT("ParseConvoScript; deserialized json"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ParseConvoScript; deserializing json failed"));
    }
}

void UConversationStarter::ParseDialogue(TSharedPtr<FJsonObject> DialogueObject)
{
    // find the exit convo button and install default exit convo behavior
    auto* HUD = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD<ARyddelmystHUD>();
    auto* ExitButton = Cast<ULambdaButton>(ConvoWidget->WidgetTree->FindWidget(TEXT("ExitButton")));
    ExitButton->LambdaEvent.BindLambda([=]() 
    {
        // exit conversation normally
        HUD->ExitConversation(ConvoWidget);
    });
    ExitButton->OnClicked.AddDynamic(ExitButton, &ULambdaButton::ExecLambda);

    for (auto DialogueElement : DialogueObject->GetArrayField(KEY_ARRAY_DIALOGUE))
    {
        UE_LOG(LogTemp, Warning, TEXT("ParseDialogue; got dialogue element in dialogues array"));
        const TSharedPtr<FJsonObject>* DialogueElementObject;
        if (DialogueElement->TryGetObject(DialogueElementObject))
        {
            UE_LOG(LogTemp, Warning, TEXT("ParseDialogue; got dialogue object"));
            FString PortraitName = (*DialogueElementObject)->GetStringField(KEY_STRING_IMAGE);
            FString PortraitPath = FString::Printf(TEXT("/Game/Ryddelmyst_Assets/Sprites/%s_Portrait_Sprite.%s_Portrait_Sprite"), *PortraitName, *PortraitName);
            UE_LOG(LogTemp, Warning, TEXT("ParseConvoScript; portraitpath is %s"), *PortraitPath);
            UPaperSprite* Portrait = LoadObject<UPaperSprite>(nullptr, *PortraitPath);
            UTextDisplayWidget* DialogueWidget{nullptr};
            
            UE_LOG(LogTemp, Warning, TEXT("ParseDialogue; portrait matching"));
            if (PortraitName.Contains(TEXT("maya"), ESearchCase::IgnoreCase, ESearchDir::FromStart))
            {
                DialogueWidget = Cast<UTextDisplayWidget>(ConvoWidget->WidgetTree->ConstructWidget(DialogueWidgetClass_Player));
            }
            else 
            {
                DialogueWidget = Cast<UTextDisplayWidget>(ConvoWidget->WidgetTree->ConstructWidget(DialogueWidgetClass_Other));
            }

            // lines field is presently required
            const TArray<TSharedPtr<FJsonValue>>& LinesArray = (*DialogueElementObject)->GetArrayField(KEY_ARRAY_LINES);
            FString LinesAggregate;
            for (auto Line : LinesArray)
            {
                LinesAggregate.Append(FString(TEXT(" ")).Append(Line->AsString()));
            }
            DialogueWidget->SetText(FText::FromString(LinesAggregate));
            DialogueWidget->SetPortrait(Portrait);
            ConvoContainer->AddChild(DialogueWidget);

            // todo: create a thoughts dialogue widget template in editor with a cheeky thought baloon border or something and italic text and load the thought text into that widget rather than cramming everything into the dialogue widget.
            UE_LOG(LogTemp, Warning, TEXT("ParseDialogue; looking at thoughts"));
            const TArray<TSharedPtr<FJsonValue>>* ThoughtsArray;
            if ((*DialogueElementObject)->TryGetArrayField(KEY_ARRAY_THOUGHTS, ThoughtsArray))
            {
                FString ThoughtsAggregate;
                UTextDisplayWidget* ThoughtsWidget{nullptr};
                // inflate thought bubbles here since those are optional and less common than spoken lines
                if (PortraitName.Contains(TEXT("maya"), ESearchCase::IgnoreCase, ESearchDir::FromStart))
                {
                    ThoughtsWidget = Cast<UTextDisplayWidget>(ConvoWidget->WidgetTree->ConstructWidget(ThoughtsWidgetClass_Player));
                }
                else 
                {
                    ThoughtsWidget = Cast<UTextDisplayWidget>(ConvoWidget->WidgetTree->ConstructWidget(ThoughtsWidgetClass_Other));
                }
                for (auto Thought : *ThoughtsArray)
                {
                    ThoughtsAggregate.Append(FString(TEXT(" ")).Append(Thought->AsString()));
                }
                ThoughtsWidget->SetText(FText::FromString(ThoughtsAggregate));
                ThoughtsWidget->SetPortrait(Portrait);
                ConvoContainer->AddChild(ThoughtsWidget);
            }
            
            UE_LOG(LogTemp, Warning, TEXT("ParseDialogue; choicing it up"));
            const TArray<TSharedPtr<FJsonValue>>* ChoicesArray;
            if ((*DialogueElementObject)->TryGetArrayField(KEY_ARRAY_CHOICES, ChoicesArray))
            {
                UUserWidget* ChoicesWidget = ConvoWidget->WidgetTree->ConstructWidget(ChoicesWidgetClass);
                ConvoContainer->AddChild(ChoicesWidget);
                auto* ChoicesList = Cast<UScrollBox>(ChoicesWidget->WidgetTree->FindWidget(TEXT("ScrollBox_Choices")));
                UE_LOG(LogTemp, Error, TEXT("ParseConvoScript; choiceslist says %p"), ChoicesList);

                // populate choiceswidget with buttons hosting the choices array text
                for (auto Choice : *ChoicesArray)
                {
                    const TSharedPtr<FJsonObject>* ChoiceObject;
                    if (Choice->TryGetObject(ChoiceObject))
                    {
                        auto ChoiceJsonObject = *ChoiceObject;
                        auto* ChoiceButton = ChoicesWidget->WidgetTree->ConstructWidget<ULambdaButton>();
                        ChoicesList->AddChild(ChoiceButton);
                        auto* ChoiceTextWidget = ChoicesWidget->WidgetTree->ConstructWidget<UTextBlock>();
                        FString ChoiceText = ChoiceJsonObject->GetStringField(KEY_STRING_TEXT);
                        ChoiceTextWidget->SetText(FText::FromString(ChoiceText));
                        ChoiceTextWidget->SetColorAndOpacity(FSlateColor(FLinearColor(0.f, 0.f, 0.f, 1.f)));
                        ChoiceButton->AddChild(ChoiceTextWidget);
						
                        if (ChoiceJsonObject->HasField(KEY_ARRAY_DIALOGUE))
                        {
                            // install subdialogue elements to OnClick lambda event   
                            ChoiceButton->LambdaEvent.BindLambda([this, ChoiceJsonObject]() 
                            {
                                UUserWidget* DividerWidget = ConvoWidget->WidgetTree->ConstructWidget(DividerWidgetClass);
                                ConvoContainer->AddChild(DividerWidget);
                                float ScrollOffset = ConvoContainer->GetScrollOffsetOfEnd() + SubtreeOffset;
                                ParseDialogue(ChoiceJsonObject);
                                UE_LOG(LogTemp, Warning, TEXT("ParseDialogue; subtreeoffset says %f and total scrolloffset says %f"), SubtreeOffset, ScrollOffset);
                                ConvoContainer->SetScrollOffset(ScrollOffset);
                            });
                        }
                        else
                        {
                            UE_LOG(LogTemp, Warning, TEXT("ParseDialogue; no subdialogue in choice: %s"), *ChoiceText);
                        }

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

            // todo: transition processing
            const TSharedPtr<FJsonObject>* TransitionObjectPtr;
            if ((*DialogueElementObject)->TryGetObjectField(KEY_OBJECT_TRANSITION, TransitionObjectPtr))
            {
                FString Clue = (*TransitionObjectPtr)->GetStringField(KEY_STRING_CLUE);
                FString Type = (*TransitionObjectPtr)->GetStringField(KEY_STRING_TYPE);
                FString Prompt = (*TransitionObjectPtr)->GetStringField(KEY_STRING_TEXT);
                if (Type.Equals(VALUE_TRANSITION_TYPE_JUMP))
                {
                    // create a transition button
                    auto* TransitionButton = ConvoWidget->WidgetTree->ConstructWidget<ULambdaButton>();
                    auto* TransitionTextWidget = ConvoWidget->WidgetTree->ConstructWidget<UTextBlock>();   
                    TransitionTextWidget->SetText(FText::FromString(Prompt));
                    TransitionTextWidget->SetColorAndOpacity(FSlateColor(FLinearColor(0.f, 0.f, 0.f, 1.f)));
                    TransitionButton->AddChild(TransitionTextWidget);
                    TransitionButton->LambdaEvent.BindLambda([this, Clue]() 
                    {
                        GameState->ClueState = Clue;
                        // re-run script selection logic  
                        ParseConversationScript(GetScript());
                        if (ScriptJsonObject)
                        {
                            ParseDialogue(ScriptJsonObject);
                        }
                        else
                        {
                            UE_LOG(LogTemp, Error, TEXT("ParseDialogue; current script object is null"));
                        }
                    });
                    TransitionButton->OnClicked.AddDynamic(TransitionButton, &ULambdaButton::ExecLambda);
                    ConvoContainer->AddChild(TransitionButton);         
                }
                else if (Type.Equals(VALUE_TRANSITION_TYPE_DEADEND))
                {
                    // the editor will not let me change the name for some reason to correct the letter case *sigh*
                    auto* ExitText = Cast<UTextBlock>(ConvoWidget->WidgetTree->FindWidget(TEXT("ExitTExt")));
                    ExitText->SetText(FText::FromString(Prompt));
                    ExitButton->LambdaEvent.BindLambda([=]() 
                    {
                        GameState->ClueState = Clue;
                        // todo: install clue derived behavior
                        // exit conversation
                        HUD->ExitConversation(ConvoWidget);
                    });
                }
            }
                        
            // todo: text input prompt processing
            
        }
    }
}