// Fill out your copyright notice in the Description page of Project Settings.


#include "ConversationStarter.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

const FString UConversationStarter::KEY_ARRAY_DIALOGUES{TEXT("dialogues")};
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

UConversationStarter::UConversationStarter()
{
    static ConstructorHelpers::FClassFinder<UUserWidget> ConvoBaseWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_ConvoBase"));
	ConvoBaseWidgetClass = ConvoBaseWidgetObj.Class;

    static ConstructorHelpers::FClassFinder<UUserWidget> DialogueWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_Dialogue"));
	DialogueWidgetClass = DialogueWidgetObj.Class;

    static ConstructorHelpers::FClassFinder<UUserWidget> ChoicesWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_Choices"));
	ChoicesWidgetClass = ChoicesWidgetObj.Class;
}

UUserWidget* UConversationStarter::ParseConversationScript(const FString& Script, ARyddelmystGameState* GameState)
{
    // todo: parse script json into UI elements added to a wrapper slate widget; for simplicity and prettyness I guess the best approach would be to create a UI asset in the editor that acts as a scrollable container and then add generated elements such as images, text, and buttons from the parsing.
    UUserWidget* ConvoWidget = CreateWidget<UUserWidget>(GetWorld(), ConvoBaseWidgetClass);
    TSharedPtr<FJsonObject> ScriptJsonObject;
    auto Reader = TJsonReaderFactory<>::Create(Script);
    if (FJsonSerializer::Deserialize(Reader, ScriptJsonObject))
    {
        auto DialogueElementsArray = ScriptJsonObject->GetArrayField(KEY_ARRAY_DIALOGUES);
        for (auto DialogueElement : DialogueElementsArray)
        {
            const TSharedPtr<FJsonObject>* DialogueObject;
            if (DialogueElement->TryGetObject(DialogueObject))
            {
                UUserWidget* DialogueWidget = ConvoWidget->WidgetTree->ConstructWidget(DialogueWidgetClass);
                // todo: populate the dialogue
                // todo: when pulling in strings from a lines or text data, use NSLOCTEXT and create it using format text;
                //  This will entail having variables in the text like FText::FormatNamed(LOCTEXT("SnippetHeader", "There are {Count} snippets in group {Name}"),TEXT("Count"), SnippetCount, TEXT("Name"), GroupNameText); which
                //  also means we'll need to manually parse out those variables from the JSON and use reflection or something
                //  to look up the appropriate GameState symbol... or just use the existing jank variable replacement I wrote 
                //  for LibraryBookWidget. Or nevermind variable templates in convo text for now.
                const TArray<TSharedPtr<FJsonValue>>& ChoicesArray = (*DialogueObject)->GetArrayField(KEY_ARRAY_CHOICES);
                if (ChoicesArray.Num() > 0)
                {
                    UUserWidget* ChoicesWidget = ConvoWidget->WidgetTree->ConstructWidget(ChoicesWidgetClass);
                    // populate choiceswidget with buttons hosting the choices array text
                    for (auto Choice : ChoicesArray)
                    {
                        auto* ChoiceButton = ChoicesWidget->WidgetTree->ConstructWidget<UButton>();
                        auto* ChoiceText = ChoiceButton->WidgetTree->ConstructWidget<UTextBlock>();
                        ChoiceText->Text = FText::FromString(Choice->AsString());
                    }
                } 
            }
        }       
    }
    // todo: when we parse out text strings from the 'lines' json elements, let's use FText and format args this time instead of our eliptical wheel version
    //  that doesn't support localization. NSLOCTEXT() is the macro you want, with namespace, key, and translatable string. Making the character names the namespace might be handy?
    return ConvoWidget;
}