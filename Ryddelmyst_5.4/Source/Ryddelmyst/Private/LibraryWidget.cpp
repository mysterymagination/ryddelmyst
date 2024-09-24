// Copyright Jeff Creswell 2024. Subject to the GPL v3.0 license.


#include "LibraryWidget.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Dom/JsonObject.h"

const FString ULibraryWidget::KEY_CONDITION_STATE_VARIABLE_NAME{TEXT("stateVarName")};
const FString ULibraryWidget::KEY_CONDITION_STATE_VARIABLE_TYPE{TEXT("type")};
const FString ULibraryWidget::KEY_CONDITION_BOOLEAN_CHAIN_OPERATOR{TEXT("booleanChainOperator")};
const FString ULibraryWidget::KEY_CONDITION_COMPARISON_OPERATOR{TEXT("comparisonOperator")};
const FString ULibraryWidget::KEY_CONDITION_PASS_VALUE{TEXT("passValue")};
const FString ULibraryWidget::VALUE_CONDITION_COMPARISON_OPERATOR_EQ{TEXT("equalTo")};
const FString ULibraryWidget::VALUE_CONDITION_COMPARISON_OPERATOR_GTE{TEXT("greaterThanOrEqualTo")};
const FString ULibraryWidget::VALUE_CONDITION_STATE_VARIABLE_TYPE_INTEGER{TEXT("integer")};
const FString ULibraryWidget::VALUE_CONDITION_STATE_VARIABLE_TYPE_BOOLEAN{TEXT("boolean")};
const FString ULibraryWidget::VALUE_CONDITION_BOOLEAN_CHAIN_OPERATOR_OR{TEXT("or")};
const FString ULibraryWidget::VALUE_CONDITION_BOOLEAN_CHAIN_OPERATOR_AND{TEXT("and")};


void ULibraryWidget::AddBook(const FLibraryBookData& Data)
{
    if (BookBank.Contains(Data.Genre))
    {
        // update case where we need to ensure the given genre book array does not contain an entry with identical information.
        bool Duplicate = false;
        for (auto Book : BookBank[Data.Genre].Books)
        {
            if (Data.Genre == ELibraryCat::Observation || Data.Genre == ELibraryCat::Diary)
            {
                if (Data.LocalizedLore.EqualToCaseIgnored(Book.LocalizedLore))
                {
                    Duplicate = true;
                }
            }
            else 
            {
                if (Data.ConversationScript.Equals(Book.ConversationScript, ESearchCase::IgnoreCase))
                {
                    Duplicate = true;
                }
            }
        }

        if (!Duplicate)
        {
            BookBank[Data.Genre].Books.Add(Data);
        }
    }
    else 
    {
        // simple insert case for novel genre.
        FLibraryBookShelf Shelf;
        Shelf.Books.Add(Data);
        BookBank.Add(Data.Genre, Shelf);
    }
}

void ULibraryWidget::PopulateUnshelved()
{
    if (BookBank.Contains(ELibraryCat::Observation))
    {
        for (auto Book : BookBank[ELibraryCat::Observation].Books)
        {
            BookDoctor(Book);
        }
        UnshelvedObservations = BookBank[ELibraryCat::Observation].Books;
    }
    if (BookBank.Contains(ELibraryCat::Conversation))
    {
        for (auto Book : BookBank[ELibraryCat::Conversation].Books)
        {
            BookDoctor(Book);
        }
        UnshelvedConversations = BookBank[ELibraryCat::Conversation].Books;
    }
    if (BookBank.Contains(ELibraryCat::Diary))
    {
        for (auto Book : BookBank[ELibraryCat::Diary].Books)
        {
            BookDoctor(Book);
        }
        UnshelvedDiaries = BookBank[ELibraryCat::Diary].Books;
    }
}

FLibraryBookData ULibraryWidget::PullUnshelved(ELibraryCat Category)
{
    FLibraryBookData data;
    switch(Category)
	{
		case ELibraryCat::Observation:
            if (UnshelvedObservations.Num() >= 1)
            {
                data = UnshelvedObservations[UnshelvedObservations.Num()-1];
                UnshelvedObservations.RemoveAt(UnshelvedObservations.Num()-1);
            }
			break;
        case ELibraryCat::Conversation:
            if (UnshelvedConversations.Num() >= 1)
            {
			    data = UnshelvedConversations[UnshelvedConversations.Num()-1];
                UnshelvedConversations.RemoveAt(UnshelvedConversations.Num()-1);
            }
			break;
        case ELibraryCat::Diary:
			if (UnshelvedDiaries.Num() >= 1)
            {
                data = UnshelvedDiaries[UnshelvedDiaries.Num()-1];
                UnshelvedDiaries.RemoveAt(UnshelvedDiaries.Num()-1);
            }
			break;
        default:
            UE_LOG(LogTemp, Error, TEXT("Unrecognized book genre %i"), Category);
            break;
	}
    return data;
}

void ULibraryWidget::BookDoctor(FLibraryBookData& Data)
{
    // search for instances of ${} template vars in the Lore of the input data
    FString LoreString = Data.LocalizedLore.ToString();
    FString VarOpenToken = TEXT("${");
    FString VarCloseToken = TEXT("}");
    while (int OpenVarIndex = LoreString.Find(VarOpenToken) != -1) 
    {
        int CloseVarIndex = LoreString.Find(VarCloseToken, ESearchCase::IgnoreCase, ESearchDir::FromStart, OpenVarIndex);
        if (CloseVarIndex != -1 && CloseVarIndex > OpenVarIndex)
        {
            // The open index will be index of the start of the open token substring, so we need to account for that in skipping to the varname content
            int VarNameStartPos = OpenVarIndex + VarOpenToken.Len();
            // The length of the VarCloseToken doesn't matter here since we just want to make sure we discount the index where it 
            // begins to catch only the end of the varname content
            int VarNameCount = CloseVarIndex - VarNameStartPos - 1;
            FString VarName = LoreString.Mid(VarNameStartPos, VarNameCount);
            FString Sub = LookupVariableSubstitution(VarName);
            // Excise the variable template substring
            LoreString.RemoveAt(OpenVarIndex, VarOpenToken.Len() + VarName.Len() + VarCloseToken.Len(), true);
            // Insert the actual variable substitution value
            LoreString.InsertAt(OpenVarIndex, Sub);
            Data.LocalizedLore = FText::FromString(LoreString);
        }
        else 
        {
            UE_LOG(LogTemp, Error, TEXT("Malformed variable sub at %i"), OpenVarIndex);
            continue;
        }
    }
}

FString ULibraryWidget::LookupVariableSubstitution(const FString& VariableName)
{
    // todo: use gamestate to figure out the appropriate variable substitution string
    //  So the idea is we have text sub lookup the relevant sub var in TextVTable.json and check each condition using
    //  the given boolean operator. If the result is true, we provide the pass substitution string. Else, we provide the fail substitution string.
    //  These substitution strings also should be scanned recursively for sub vars to support nonlinear (as in gameplay, not algebra) substitution chains.
    //  Actually reading the game state variable values is going to need some kind of string varname to actual cpp var
    //  mapping; this could also map to functions by name if we wanna get real weird with it, but that's probably not necessary for now.
    if (TextVTableJSONPath.IsEmpty())
    {
        TextVTableJSONPath = FPaths::ProjectContentDir().Append(TEXT("Ryddelmyst_Assets/Text/TextVTable.json"));
    }

    FString TextVTableJSON;
    if(FFileHelper::LoadFileToString(TextVTableJSON, *TextVTableJSONPath))
    {
        UE_LOG(LogTemp, Log, TEXT("LookupVariableSub; json path is %s and contents says %s"), *TextVTableJSONPath, *TextVTableJSON);
    }
    else 
    {
        UE_LOG(LogTemp, Log, TEXT("LookupVariableSub; failed to load json contents of file path %s"), *TextVTableJSONPath);
    }

    TSharedPtr<FJsonObject> TextVTableJsonObject;
    auto Reader = TJsonReaderFactory<>::Create(TextVTableJSON);
    if (FJsonSerializer::Deserialize(Reader, TextVTableJsonObject))
    {
        const TSharedPtr<FJsonObject>* TopLevelVarEntry;
        TextVTableJsonObject->TryGetObjectField(VariableName, TopLevelVarEntry);
        const TArray<TSharedPtr<FJsonValue>>* ConditionsArray;
        (*TopLevelVarEntry)->TryGetArrayField(TEXT("conditions"), ConditionsArray);
        // todo: parse out condition vars and call 'em from predicate map based on metadata attributes
        bool AllConditionsPassed = false;
        for (auto Condition : *ConditionsArray)
        {
            bool ConditionPassed = false;
            const TSharedPtr<FJsonObject>* ConditionObject;
            if (Condition->TryGetObject(ConditionObject))
            {
                FString StateVarName = (*ConditionObject)->GetStringField(KEY_CONDITION_STATE_VARIABLE_NAME);
                FString StateVarType = (*ConditionObject)->GetStringField(KEY_CONDITION_STATE_VARIABLE_TYPE);
                auto GameState = Cast<RyddelmystGameState>(GetWorld()->GetGameState());
                if (StateVarType == VALUE_CONDITION_STATE_VARIABLE_TYPE_BOOLEAN)
                {
                    bool* StateValue_ptr = GameState->StatesMapBool.Find(StateVarName);
                    if (StateValue_ptr)
                    {
                        // parse out the pass condition value and comparison operator (which for boolean type can only be EQ), and compare accordingly to actual state value
                        FString ComparisonOp = (*ConditionObject)->GetStringField(KEY_CONDITION_COMPARISON_OPERATOR);
                        if (ComparisonOp == VALUE_CONDITION_COMPARISON_OPERATOR_EQ)
                        {
                            FString PassVal = (*ConditionObject)->GetStringField(KEY_CONDITION_PASS_VALUE);
                            if (PassVal == "true")
                            {
                                ConditionPassed = *StateValue_ptr;
                            }
                            else 
                            {
                                ConditionPassed = !*StateValue_ptr;
                            }
                        }
                    }
                }

                FString BooleanChainOp;
                if ((*ConditionObject)->TryGetStringField(KEY_CONDITION_BOOLEAN_CHAIN_OPERATOR, BooleanChainOp))
                {
                    if (BooleanChainOp == "and")
                    {
                        AllConditionsPassed = AllConditionsPassed && ConditionPassed;
                    }
                    else if (BooleanChainOp == "or")
                    {
                        AllConditionsPassed = AllConditionsPassed || ConditionPassed;
                    }
                }
                else 
                {
                    AllConditionsPassed = ConditionPassed;
                }
            }
            else 
            {
                UE_LOG(LogTemp, Error, TEXT("LookupVariableSub; failed to coerce a Condition FJsonValue to JSON object. It's type comes back as %s"), *Condition->GetType());
            }
        }
        // todo: run pass/fail sub string through StringDoctor() for recursive var sub
        // todo: return the fully substituted string
    }
        
    return TEXT("FillInLater");
}