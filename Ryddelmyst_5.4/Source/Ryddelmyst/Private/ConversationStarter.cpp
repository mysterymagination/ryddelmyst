// Fill out your copyright notice in the Description page of Project Settings.


#include "ConversationStarter.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/PanelWidget.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"


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
    UScrollBox* ScrollBox = Cast<UScrollBox>(ConvoWidget->WidgetTree->FindWidget(TEXT("DialogueScrollBox")));
    UUserWidget* ChoicesWidget = ConvoWidget->WidgetTree->ConstructWidget(ChoicesWidgetClass);
    ScrollBox->AddChild(ChoicesWidget);
    auto* ChoicesList = Cast<UVerticalBox>(ChoicesWidget->WidgetTree->FindWidget(TEXT("ChoicesList")));
    UE_LOG(LogTemp, Error, TEXT("ParseConvoScript; choiceslist says %p"), ChoicesList);

    /*
    auto* ChoiceButton = ChoicesWidget->WidgetTree->ConstructWidget<UButton>();
    ChoicesList->AddChild(ChoiceButton);
    auto* ChoiceText = ChoicesWidget->WidgetTree->ConstructWidget<UTextBlock>();
    ChoiceButton->AddChild(ChoiceText);
    ChoiceText->SetText(FText::FromString(TEXT("Fuzlor")));
    */
    
    TSharedPtr<FJsonObject> ScriptJsonObject;
    auto Reader = TJsonReaderFactory<>::Create(Script);
    if (FJsonSerializer::Deserialize(Reader, ScriptJsonObject))
    {
        UE_LOG(LogTemp, Warning, TEXT("ParseConvoScript; deserialized json"));

        auto DialogueElementsArray = ScriptJsonObject->GetArrayField(KEY_ARRAY_DIALOGUE);
        UE_LOG(LogTemp, Warning, TEXT("ParseConvoScript; dialogue array has %d elements"), DialogueElementsArray.Num());

        for (auto DialogueElement : DialogueElementsArray)
        {
            UE_LOG(LogTemp, Warning, TEXT("ParseConvoScript; got dialogue element in dialogues array"));

            const TSharedPtr<FJsonObject>* DialogueObject;
            if (DialogueElement->TryGetObject(DialogueObject))
            {
                UE_LOG(LogTemp, Warning, TEXT("ParseConvoScript; got dialogue object"));
            
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
                    // populate choiceswidget with buttons hosting the choices array text
                    for (auto Choice : ChoicesArray)
                    {
                        const TSharedPtr<FJsonObject>* ChoiceObject;
                        if (Choice->TryGetObject(ChoiceObject))
                        {
                            auto* ChoiceButton = ChoicesWidget->WidgetTree->ConstructWidget<UButton>();
                            ChoicesList->AddChild(ChoiceButton);
                            auto* ChoiceTextWidget = ChoicesWidget->WidgetTree->ConstructWidget<UTextBlock>();
                            FString ChoiceText = (*ChoiceObject)->GetStringField(KEY_STRING_TEXT);
                            ChoiceTextWidget->Text = FText::FromString(ChoiceText);
                            ChoiceButton->AddChild(ChoiceTextWidget);
                            // todo: install onclick based on presence of nested dialogue trees, jumps, deadends etc.
                        }
                        else
                        {
                            UE_LOG(LogTemp, Error, TEXT("ParseConvoScript; choice jsonval could not be converted to object"));
                        }
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("ParseConvoScript; no choices found"));
                }
            }
        }       
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ParseConvoScript; deserializing json failed"));
    }
    // todo: when we parse out text strings from the 'lines' json elements, let's use FText and format args this time instead of our eliptical wheel version
    //  that doesn't support localization. NSLOCTEXT() is the macro you want, with namespace, key, and translatable string. Making the character names the namespace might be handy?
    return ConvoWidget;
}