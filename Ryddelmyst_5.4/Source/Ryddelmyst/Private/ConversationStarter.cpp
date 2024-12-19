// Fill out your copyright notice in the Description page of Project Settings.


#include "ConversationStarter.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonWriter.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "TextDisplayWidget.h"
#include "Kismet/GameplayStatics.h"
#include "RyddelmystGameInstance.h"
#include "Components/EditableText.h"
#include "RyddelmystCharacter.h"
#include "RyddelmystHUD.h"
#include "GenericPlatform/GenericPlatformTime.h"
#include "LibraryBookWidget.h"
#include "LibraryWidget.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

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
const FString UConversationStarter::MATCHER_PLAYER_MAYA{TEXT("maya")};
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

    static ConstructorHelpers::FClassFinder<UUserWidget> ButtonWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_Button"));
	ButtonWidgetClass = ButtonWidgetObj.Class;

    static ConstructorHelpers::FClassFinder<UUserWidget> TextInputWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_TextInput"));
	TextInputWidgetClass = TextInputWidgetObj.Class;
}

void UConversationStarter::Init(const FString& _ConvoTx, const FString& _ConvoRx, FName _ClosestBone, ARyddelmystGameState* _GameState)
{
    ConvoTx = _ConvoTx;
    ConvoRx = _ConvoRx;
    ClosestBone = _ClosestBone;
    GameState = _GameState;
}

void UConversationStarter::SaveConversation(const FString& ConvoName)
{
    // step through the ConvoWidget widgettree and write equivalent JSON for each dialogue UI element, 
    //  to be loaded from quest log at any time using PaseConversationScript(). Skip wrapper parts of convowidget like exit button?
    TSharedPtr<FJsonObject> ConvoJsonObject = MakeShareable(new FJsonObject());
    TArray<TSharedPtr<FJsonValue>> DialogueElements;
    ConvoWidget->WidgetTree->ForEachWidget([&](UWidget* Widget) {
        // look at widget type and/or name to figure out:
        //  1. Should it be included in the logged convo? e.g. dialogue text and portrait but not choices buttons.
        //  2. What JSON subobject should be written to ConvoJsonObject, and what should its contents be? e.g. dialouge element with text and portrait.
        // write player dialogue widget element out to json
        auto* DialogueWidget = Cast<UTextDisplayWidget>(Widget);
        if (DialogueWidget)
        {
            // lookbehind for thoughts and operate on the tail of DialogueElements array, sticking a thoughts array onto 'im if relevant.
            if (DialogueWidget->IsA(ThoughtsWidgetClass_Player) || DialogueWidget->IsA(ThoughtsWidgetClass_Other))
            {
                TSharedPtr<FJsonValue> ThoughtText = MakeShareable(new FJsonValueString(DialogueWidget->GetText().ToString()));   
                TArray<TSharedPtr<FJsonValue>> ThoughtsArray;
                ThoughtsArray.Add(ThoughtText); 
                auto DialogueElementTail = DialogueElements.Last()->AsObject();
                DialogueElementTail->SetArrayField(KEY_ARRAY_THOUGHTS, ThoughtsArray);
            }
            else
            {
                // create a jsonvalueobject to be the dialogue element
                TSharedPtr<FJsonObject> DialogueJsonObject = MakeShareable(new FJsonObject());
                // extract portrait name code
                DialogueJsonObject->SetStringField(KEY_STRING_IMAGE, DialogueWidget->GetPortraitName());
                TSharedPtr<FJsonValue> DialogueElement = MakeShareable(new FJsonValueObject(DialogueJsonObject));    
                DialogueElements.Add(DialogueElement);
                // we drop the array structure when we write the text to the widget, so just go ahead and store the entire text as a single array element.
                // I only used an array of lines to make the json easier to read and debug.
                TSharedPtr<FJsonValue> FullText = MakeShareable(new FJsonValueString(DialogueWidget->GetText().ToString()));   
                TArray<TSharedPtr<FJsonValue>> LinesArray;
                LinesArray.Add(FullText); 
                DialogueJsonObject->SetArrayField(KEY_ARRAY_LINES, LinesArray);
            }
        }
    });

    // populate the dialogue array with the generated dialogue elements
    ConvoJsonObject->SetArrayField(KEY_ARRAY_DIALOGUE, DialogueElements);

    // write the generated JSON as a convo script in an FLibraryBookData.ConversationScript, followed by ULibraryWidget::AddLore(bookdata). That way we'll have the convo script added to Library.BookBank, which is the first step in having the library widget display it.
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(ConvoJsonObject.ToSharedRef(), Writer);
    FLibraryBookData Data;
    Data.LocalizedTitle = FText::FromString(ConvoName);
    Data.ConversationScript = OutputString;
    auto* HUD = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD<ARyddelmystHUD>();
    HUD->AddLore(Data);
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

void UConversationStarter::DeriveDeadend(const FString& Clue)
{
    // parse apart/switch over clue text to choose game behavior
    if (Clue == ARyddelmystGameState::STATE_CLUE_YVYTEPH_MASTERMIND_REJECTED_ANGEL)
    {
        // todo: find actor BP_Yvyteph_Mastermind_ConvoActor
        // todo: set player position a bit -Y relative to mastermind actor
        FVector DownstairsPos{-250.0f, -370.0, -470.0};
        UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->SetActorLocation(DownstairsPos);
        ExecuteDefaultExitBehavior();
    }
    else if (Clue == ARyddelmystGameState::STATE_CLUE_ENDING_PRACTICAL_PAWN)
    {
        UE_LOG(LogTemp, Warning, TEXT("DeriveDeadend; %s bad ending!"), Clue);
        // todo: lower conversation UI
        // todo: bring up diary entry UI with entry about new sandbox world suspicions
        // todo: after player dismisses the diary entry UI, roll credits
    }
    else if (
        Clue == ARyddelmystGameState::STATE_CLUE_ENDING_WILD_DETERMINATION_HOMEWARD || 
        Clue == ARyddelmystGameState::STATE_CLUE_ENDING_WILD_DETERMINATION_HEAVENWARD ||
        Clue == ARyddelmystGameState::STATE_CLUE_ENDING_GOOD_DETERMINATION_HOMEWARD ||
        Clue == ARyddelmystGameState::STATE_CLUE_ENDING_GOOD_DETERMINATION_HEAVENWARD 
    )
    {
        UE_LOG(LogTemp, Warning, TEXT("DeriveDeadend; %s good or 'good' ending!"), Clue);
        // todo: lower conversation UI; maybe run default exit behavior? Depends if we need the game unpaused to animate the credits roll. If so, then we'll want to take the player to an empty map so the game can be unpaused with nothing happening.
        // todo: roll credits
    }
    else
    {
        FVector StartPos = Cast<URyddelmystGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->PlayerStartPosition;
        UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->SetActorLocation(StartPos);
        ExecuteDefaultExitBehavior();
    }
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
        else if (GameState->ClueState == ARyddelmystGameState::STATE_CLUE_MAYA_EVIL_DETERMINATION)
        {
            ConvoScriptName = TEXT("Undercarriage_Urgent_Desire_Yvyteph_Mastermind.json");
        }
        else if (GameState->ClueState == ARyddelmystGameState::STATE_CLUE_MAYA_GOOD_DETERMINATION)
        {
            ConvoScriptName = TEXT("Maya_Friendly_Mastery.json");
        }
        else if (GameState->WoodEggBeholden)
        {
            ConvoScriptName = TEXT("Undercarriage_Wood_Egg_Knowledge_Yvyteph_Mastermind.json");
        }
        else
        {
            ConvoScriptName = TEXT("Undercarriage_Ejector_Yvyteph_Mastermind.json");
        }
	}
	else if (CharacterName.Equals(MATCHER_YVYTEPH_FONTOFFERTILITY))
	{
		ConvoScriptName = TEXT("Intro_Yvyteph_FontOfFertility.json");
	}
    else if (CharacterName.Equals(MATCHER_YVYTEPH_WILDFORM))
	{
		ConvoScriptName = TEXT("Intro_Yvyteph_WildForm.json");
	}
    else if (CharacterName.Equals(MATCHER_YVYTEPH_WILDFORMSHARP))
	{
		ConvoScriptName = TEXT("Intro_Yvyteph_WildFormSharp.json");
	}
    else if (CharacterName.Equals(MATCHER_QYVNILY_WILDFORM))
	{
		ConvoScriptName = TEXT("Intro_Qyvnily_WildForm.json");
        if (GameState->ClueState == ARyddelmystGameState::STATE_CLUE_QYVNILY_WILD_PERSPECTIVE)
        {
            ConvoScriptName = TEXT("Qyvnily_WildForm_Perspective.json");
        }
	}
    else if (CharacterName.Equals(MATCHER_QYVNILY_GLORYFORM))
	{
		ConvoScriptName = TEXT("Intro_Qyvnily_GloryForm.json");
        if (GameState->ClueState == ARyddelmystGameState::STATE_CLUE_QYVNILY_GLORY_PERSPECTIVE)
        {
            ConvoScriptName = TEXT("Qyvnily_GloryForm_Perspective.json");
        }
	}
    else if (CharacterName.Equals(MATCHER_QYVNILY_GLORYFORMRAGE))
	{
		ConvoScriptName = TEXT("Intro_Qyvnily_GloryFormRage.json");
	}
	else if (CharacterName.Equals(MATCHER_QYVNILY_WILDFLOWER))
	{
        ConvoScriptName = TEXT("Intro_Qyvnily_WildFlower.json");
        if (GameState->ClueState == ARyddelmystGameState::STATE_CLUE_INPUT_QYVNILY_DANCE)
		{
            for (auto Token : GameState->MatingDanceTokens)
            {
                if (GameState->UserInputValue.Contains(Token, ESearchCase::CaseSensitive, ESearchDir::FromStart))
                {
                    ConvoScriptName = TEXT("Qyvnily_WildFlower_Bonding.json");
                    auto* Player = Cast<ARyddelmystCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
                    Player->FullHeal();
                }
            }

            if (ConvoScriptName != TEXT("Qyvnily_WildFlower_Bonding.json"))
            {
                ConvoScriptName = TEXT("Qyvnily_WildFlower_Attempted_Bonding.json");
            }
        }
	}
    else if (CharacterName.Equals(MATCHER_TEST))
	{
        if (GameState->ClueState == ARyddelmystGameState::STATE_CLUE_INPUT_TEST_ANSWER1)
		{
			if (
				GameState->UserInputValue.Contains(ARyddelmystGameState::VALUE_INPUT_EGG_TOKEN) && 
				GameState->UserInputValue.Contains(ARyddelmystGameState::VALUE_INPUT_LAVA_TOKEN)
			)
            {
                ConvoScriptName = TEXT("TestJSON_InputMatch.json");
            }
            else
            {
                ConvoScriptName = TEXT("TestJSON_InputMismatch.json");
            }

            // input value consumed
            GameState->UserInputValue = TEXT("");
        }
        else
        {
		    ConvoScriptName = TEXT("TestJSON.json");
        }
	}
	
    // clue state consumed
    GameState->ClueState = TEXT("");
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
    else if (ActorName.Contains(MATCHER_PLAYER_MAYA, ESearchCase::IgnoreCase, ESearchDir::FromStart))
	{
		CharacterName = MATCHER_PLAYER_MAYA;
	}
	
    // override with test script if actor name suggests it
    if (ActorName.Contains(MATCHER_TEST, ESearchCase::IgnoreCase, ESearchDir::FromStart))
	{
		CharacterName = MATCHER_TEST;
	}

    return CharacterName;
}

void UConversationStarter::ExecuteDefaultExitBehavior()
{
    auto* HUD = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD<ARyddelmystHUD>();
    FString ConvoTrace = MatchCharacter(ConvoTx).Append(TEXT(" to ")).Append(MatchCharacter(ConvoRx)).Append(TEXT(" ")).Append(PrettyTimestamp()).Append(TEXT(".txt"));
    SaveConversation(ConvoTrace);
    HUD->ExitConversation(ConvoWidget);
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->SetShowMouseCursor(false);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
    Cast<URyddelmystGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->Unpause();
}

void UConversationStarter::InstallDefaultExitBehavior()
{
    // find the exit convo button and install default exit convo behavior
    auto* ExitButton = Cast<ULambdaButton>(ConvoWidget->WidgetTree->FindWidget(TEXT("ExitButton")));
    ExitButton->LambdaEvent.BindLambda([this]() 
    {
        UE_LOG(LogTemp, Warning, TEXT("InstallDefaultExitBehavior; exit saveconvo"));
        // exit conversation normally
        ExecuteDefaultExitBehavior();
    });
    ExitButton->OnClicked.AddDynamic(ExitButton, &ULambdaButton::ExecLambda);
}

void UConversationStarter::InstallQuestLogExitBehavior()
{
    auto* ExitButton = Cast<ULambdaButton>(ConvoWidget->WidgetTree->FindWidget(TEXT("ExitButton")));
    ExitButton->LambdaEvent.BindLambda([this]() 
    {
        UE_LOG(LogTemp, Warning, TEXT("InstallQuestLogExitBehavior; exit saveconvo"));
        // exit conversation UI only
        auto* HUD = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD<ARyddelmystHUD>();
        HUD->ExitConversation(ConvoWidget);
    });
    ExitButton->OnClicked.AddDynamic(ExitButton, &ULambdaButton::ExecLambda);
}

UUserWidget* UConversationStarter::GenerateConversationUI(const FString& Script)
{
    /// UI setup ///
    ConvoWidget = CreateWidget<UUserWidget>(GetWorld(), ConvoBaseWidgetClass);
    ConvoContainer = Cast<UScrollBox>(ConvoWidget->WidgetTree->FindWidget(TEXT("DialogueScrollBox")));
    InstallDefaultExitBehavior();

    /// script parsing ///
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
            DialogueWidget->SetPortraitName(PortraitName);
            ConvoContainer->AddChild(DialogueWidget);

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
                ThoughtsWidget->SetPortraitName(PortraitName);
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
                                AddDivider();
                                ParseDialogue(ChoiceJsonObject);
                                PageDown();
                            });
                        }
                        else
                        {
                            UE_LOG(LogTemp, Warning, TEXT("ParseDialogue; no subdialogue in choice: %s"), *ChoiceText);
                            // install behavior for any transition this choice might define
                            ProcessChoiceTransition(ChoiceJsonObject, ChoiceButton);
                        }

                        // install OnClicked behavior, instructing it to simply exec the lambda
						ChoiceButton->OnClicked.AddDynamic(ChoiceButton, &ULambdaButton::ExecLambda);
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

            // install behavior for any transition the dialogue object might define
            ProcessDialogueTransition(*DialogueElementObject);
        }
    }
}

void UConversationStarter::ProcessChoiceTransition(TSharedPtr<FJsonObject> ChoiceObject, ULambdaButton* ChoiceButton)
{
    // transition processing
    const TSharedPtr<FJsonObject>* TransitionObjectPtr;
    if (ChoiceObject->TryGetObjectField(KEY_OBJECT_TRANSITION, TransitionObjectPtr))
    {
        FString Clue = (*TransitionObjectPtr)->GetStringField(KEY_STRING_CLUE);
        FString Type = (*TransitionObjectPtr)->GetStringField(KEY_STRING_TYPE);
        if (Type.Equals(VALUE_TRANSITION_TYPE_JUMP))
        {
            ChoiceButton->LambdaEvent.BindLambda([this, Clue]() 
            {
                GameState->ClueState = Clue;
                
                // re-run script selection logic  
                ParseConversationScript(GetScript());
                if (ScriptJsonObject)
                {
                    AddDivider();
                    ParseDialogue(ScriptJsonObject);
                    PageDown();
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("ProcessChoiceTransition; current script object is null"));
                }
            });        
        }
        else if (Type.Equals(VALUE_TRANSITION_TYPE_DEADEND))
        {
            ChoiceButton->LambdaEvent.BindLambda([this, Clue]() 
            {
                UE_LOG(LogTemp, Warning, TEXT("ProcessChoiceTransition; deadend saveconvo"));
                GameState->ClueState = Clue;
                // install clue derived behavior e.g. teleport player back to starting table.
                DeriveDeadend(Clue);
                ExecuteDefaultExitBehavior();
            });
        }
    }
}

void UConversationStarter::ProcessDialogueTransition(TSharedPtr<FJsonObject> DialogueObject)
{
    // transition processing
    const TSharedPtr<FJsonObject>* TransitionObjectPtr;
    if (DialogueObject->TryGetObjectField(KEY_OBJECT_TRANSITION, TransitionObjectPtr))
    {
        // text input prompt processing
        FString TextInputHint;
        UEditableText* TextInput = nullptr;
        if ((*TransitionObjectPtr)->TryGetStringField(KEY_STRING_INPUT, TextInputHint))
        {
            UE_LOG(LogTemp, Warning, TEXT("ProcessTransition; adding input UI with hint %s"), *TextInputHint);
            auto* TextInputWidget = ConvoWidget->WidgetTree->ConstructWidget(TextInputWidgetClass);
            TextInput = Cast<UEditableText>(TextInputWidget->WidgetTree->FindWidget(TEXT("TextInput")));
            TextInput->SetHintText(FText::FromString(TextInputHint));
            ConvoContainer->AddChild(TextInputWidget);
        }
        FString Clue = (*TransitionObjectPtr)->GetStringField(KEY_STRING_CLUE);
        FString Type = (*TransitionObjectPtr)->GetStringField(KEY_STRING_TYPE);
        FString Prompt = (*TransitionObjectPtr)->GetStringField(KEY_STRING_TEXT);
        if (Type.Equals(VALUE_TRANSITION_TYPE_JUMP))
        {
            // create a transition button
            auto* TransitionButtonWidget = ConvoWidget->WidgetTree->ConstructWidget(ButtonWidgetClass);
            auto* TransitionButton = Cast<ULambdaButton>(TransitionButtonWidget->WidgetTree->FindWidget(TEXT("Button")));
            auto* TransitionText = Cast<UTextBlock>(TransitionButtonWidget->WidgetTree->FindWidget(TEXT("ButtonText")));   
            TransitionText->SetText(FText::FromString(Prompt));
            TransitionText->SetColorAndOpacity(FSlateColor(FLinearColor(0.f, 0.f, 0.f, 1.f)));
            TransitionButton->AddChild(TransitionText);
            TransitionButton->LambdaEvent.BindLambda([this, Clue, TextInput]() 
            {
                GameState->ClueState = Clue;
                if (TextInput) 
                {
                    GameState->UserInputValue = TextInput->GetText().ToString();
                }
                // re-run script selection logic  
                ParseConversationScript(GetScript());
                if (ScriptJsonObject)
                {
                    AddDivider();
                    ParseDialogue(ScriptJsonObject);
                    PageDown();
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("ParseDialogue; current script object is null"));
                }
            });
            TransitionButton->OnClicked.AddDynamic(TransitionButton, &ULambdaButton::ExecLambda);
            ConvoContainer->AddChild(TransitionButtonWidget);         
        }
        else if (Type.Equals(VALUE_TRANSITION_TYPE_DEADEND))
        {
            // the editor will not let me change the name for some reason to correct the letter case *sigh*
            auto* ExitText = Cast<UTextBlock>(ConvoWidget->WidgetTree->FindWidget(TEXT("ExitTExt")));
            ExitText->SetText(FText::FromString(Prompt));
            auto* ExitButton = Cast<ULambdaButton>(ConvoWidget->WidgetTree->FindWidget(TEXT("ExitButton")));
            ExitButton->LambdaEvent.BindLambda([this, Clue]() 
            {
                UE_LOG(LogTemp, Warning, TEXT("ParseDialogue; deadend saveconvo"));
                GameState->ClueState = Clue;
                // install clue derived behavior e.g. teleport player back to starting table.
                DeriveDeadend(Clue);
            });
        }
    }
}

void UConversationStarter::AddDivider()
{
    UUserWidget* DividerWidget = ConvoWidget->WidgetTree->ConstructWidget(DividerWidgetClass);
    ConvoContainer->AddChild(DividerWidget);
}

void UConversationStarter::PageDown()
{
    float ScrollOffset = ConvoContainer->GetScrollOffsetOfEnd() + SubtreeOffset;
    ConvoContainer->SetScrollOffset(ScrollOffset);
}

FString UConversationStarter::PrettyTimestamp()
{
    int32 Year, Month, DayOfWeek, Day, Hour, Min, Sec, MSec;
    FGenericPlatformTime::SystemTime(Year, Month, DayOfWeek, Day, Hour, Min, Sec, MSec);
    FString Timestamp = FString::FromInt(Year).Append(TEXT("-")).Append(FString::FromInt(Month)).Append(TEXT("-")).Append(FString::FromInt(Day)).Append(TEXT("-")).Append(FString::FromInt(Hour)).Append(TEXT("-")).Append(FString::FromInt(Min)).Append(TEXT("-")).Append(FString::FromInt(Sec));
    return Timestamp;
}