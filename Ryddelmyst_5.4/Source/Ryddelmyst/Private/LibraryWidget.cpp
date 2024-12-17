// Copyright Jeff Creswell 2024. Subject to the GPL v3.0 license.


#include "LibraryWidget.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Dom/JsonObject.h"
#include "RyddelmystGameState.h"
#include "Kismet/KismetStringLibrary.h"

const FString ULibraryWidget::KEY_CONDITIONS{TEXT("conditions")};
const FString ULibraryWidget::KEY_CONDITION_STATE_VARIABLE_NAME{TEXT("stateVarName")};
const FString ULibraryWidget::KEY_CONDITION_STATE_VARIABLE_TYPE{TEXT("type")};
const FString ULibraryWidget::KEY_CONDITION_BOOLEAN_CHAIN_OPERATOR{TEXT("booleanChainOperator")};
const FString ULibraryWidget::KEY_CONDITION_COMPARISON_OPERATOR{TEXT("comparisonOperator")};
const FString ULibraryWidget::KEY_CONDITION_PASS_VALUE{TEXT("passValue")};
const FString ULibraryWidget::KEY_PASS_SUBSTITUTION{TEXT("passSubstitution")};
const FString ULibraryWidget::KEY_FAIL_SUBSTITUTION{TEXT("failSubstitution")};
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
        UnshelvedObservations = BookBank[ELibraryCat::Observation].Books;
    }
    if (BookBank.Contains(ELibraryCat::Conversation))
    {
        UnshelvedConversations = BookBank[ELibraryCat::Conversation].Books;
    }
    if (BookBank.Contains(ELibraryCat::Diary))
    {
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
    FString LoreString = Data.LocalizedLore.ToString();
    StringDoctor(LoreString);
    // todo: this loses any localization; not sure how ye'd look up localized substitutions. perhaps I should have just used the existing FText::Format() APIs for this? XD
    Data.LocalizedLore = FText::FromString(LoreString);
}

void ULibraryWidget::StringDoctor(FString& LoreString)
{
    // search for instances of ${} template vars in the Lore of the input data
    FString VarOpenToken = TEXT("${");
    FString VarCloseToken = TEXT("}");
    int OpenVarIndex = LoreString.Find(VarOpenToken);
    while (OpenVarIndex != -1) 
    {
        int CloseVarIndex = LoreString.Find(VarCloseToken, ESearchCase::IgnoreCase, ESearchDir::FromStart, OpenVarIndex);
        if (CloseVarIndex != -1 && CloseVarIndex > OpenVarIndex)
        {
            // The open index will be index of the start of the open token substring, so we need to account for that in skipping to the varname content
            int VarNameStartPos = OpenVarIndex + VarOpenToken.Len();
            // The length of the VarCloseToken doesn't matter here since we just want to make sure we discount the index where it 
            // begins to catch only the end of the varname content
            int VarNameCount = CloseVarIndex - VarNameStartPos;
            FString VarName = LoreString.Mid(VarNameStartPos, VarNameCount);
            FString Sub = LookupVariableSubstitution(VarName);
            UE_LOG(LogTemp, Log, TEXT("StringDoctor; parsed out var name is: %s and sub is: %s"), *VarName, *Sub);
            // Excise the variable template substring
            LoreString.RemoveAt(OpenVarIndex, VarOpenToken.Len() + VarName.Len() + VarCloseToken.Len(), true);
            // Insert the actual variable substitution value
            LoreString.InsertAt(OpenVarIndex, Sub);
        }
        else 
        {
            UE_LOG(LogTemp, Error, TEXT("StringDoctor; Malformed variable sub at %i"), OpenVarIndex);
            continue;
        }
        // on to the next one, if still applicable
        OpenVarIndex = LoreString.Find(VarOpenToken);
    }
}

FString ULibraryWidget::LookupVariableSubstitution(const FString& VariableName)
{
    UE_LOG(LogTemp, Log, TEXT("LookupVariableSubstitution; var name is: %s"), *VariableName);
    // *shh* nobody tell him about FText::Format() XD I wanted practice with UE JSON before the scary convo script processor ok?!

    if (GameState)
    {
        // use gamestate to figure out the appropriate variable substitution string:
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
        if (FFileHelper::LoadFileToString(TextVTableJSON, *TextVTableJSONPath))
        {
            TSharedPtr<FJsonObject> TextVTableJsonObject;
            auto Reader = TJsonReaderFactory<>::Create(TextVTableJSON);
            if (FJsonSerializer::Deserialize(Reader, TextVTableJsonObject))
            {
                auto TopLevelVarEntry = TextVTableJsonObject->GetObjectField(VariableName);
                auto ConditionsArray = TopLevelVarEntry->GetArrayField(KEY_CONDITIONS);
                // parse out condition vars and look 'em up from predicate map based on metadata attributes
                bool AllConditionsPassed = false;
                for (auto Condition : ConditionsArray)
                {
                    UE_LOG(LogTemp, Log, TEXT("LookupVariableSub; stepping through conditions array."));
                    bool ConditionPassed = false;
                    const TSharedPtr<FJsonObject>* ConditionObject;
                    if (Condition->TryGetObject(ConditionObject))
                    {
                        UE_LOG(LogTemp, Log, TEXT("LookupVariableSub; parsed condition into object."));

                        // parse out the pass condition value and comparison operator, and compare accordingly to actual state value
                        FString StateVarName = (*ConditionObject)->GetStringField(KEY_CONDITION_STATE_VARIABLE_NAME);
                        FString StateVarType = (*ConditionObject)->GetStringField(KEY_CONDITION_STATE_VARIABLE_TYPE);
                        FString ComparisonOp = (*ConditionObject)->GetStringField(KEY_CONDITION_COMPARISON_OPERATOR);
                        FString PassVal = (*ConditionObject)->GetStringField(KEY_CONDITION_PASS_VALUE);
                        if (StateVarType == VALUE_CONDITION_STATE_VARIABLE_TYPE_BOOLEAN)
                        {
                            bool* StateValue_ptr = GameState->StatesMapBool.Find(StateVarName);
                            if (StateValue_ptr)
                            {
                                if (ComparisonOp == VALUE_CONDITION_COMPARISON_OPERATOR_EQ)
                                {
                                    if (PassVal == "true")
                                    {
                                        ConditionPassed = *StateValue_ptr;
                                    }
                                    else
                                    {
                                        ConditionPassed = !*StateValue_ptr;
                                    }
                                    UE_LOG(LogTemp, Log, 
                                        TEXT("LookupVariableSub; for variablename %s's condition state var %s, bool eq comparison condition passed says %d based on passval of %s and state val of %d"), 
                                        *VariableName, 
                                        *StateVarName, 
                                        ConditionPassed, 
                                        *PassVal, 
                                        *StateValue_ptr
                                    );
                                }
                            }
                        }
                        else if (StateVarType == VALUE_CONDITION_STATE_VARIABLE_TYPE_INTEGER)
                        {
                            int* StateValue_ptr = GameState->StatesMapInt.Find(StateVarName);
                            if (StateValue_ptr)
                            {
                                if (ComparisonOp == VALUE_CONDITION_COMPARISON_OPERATOR_EQ)
                                {
                                    ConditionPassed = *StateValue_ptr == UKismetStringLibrary::Conv_StringToInt(PassVal);
                                    UE_LOG(LogTemp, Log, 
                                        TEXT("LookupVariableSub; for variablename %s's condition state var %s, int eq comparison condition passed says %d based on passval of %s and state val of %d"), 
                                        *VariableName, 
                                        *StateVarName, 
                                        ConditionPassed, 
                                        *PassVal, 
                                        *StateValue_ptr
                                    );
                                }
                                else if (ComparisonOp == VALUE_CONDITION_COMPARISON_OPERATOR_GTE)
                                {
                                    ConditionPassed = *StateValue_ptr >= UKismetStringLibrary::Conv_StringToInt(PassVal);
                                    UE_LOG(LogTemp, Log, 
                                        TEXT("LookupVariableSub; for variablename %s's condition state var %s, int gte comparison condition passed says %d based on passval of %s and state val of %d"), 
                                        *VariableName, 
                                        *StateVarName, 
                                        ConditionPassed, 
                                        *PassVal, 
                                        *StateValue_ptr
                                    );
                                }
                            }
                        }

                        FString BooleanChainOp;
                        if ((*ConditionObject)->TryGetStringField(KEY_CONDITION_BOOLEAN_CHAIN_OPERATOR, BooleanChainOp))
                        {
                            if (BooleanChainOp == "and")
                            {
                                AllConditionsPassed = AllConditionsPassed && ConditionPassed;
                                UE_LOG(LogTemp, Log, 
                                    TEXT("LookupVariableSub; for variablename %s's condition state var %s, 'and' chain gives allconditions passed as %d"), 
                                    *VariableName, 
                                    *StateVarName, 
                                    AllConditionsPassed
                                );
                            }
                            else if (BooleanChainOp == "or")
                            {
                                AllConditionsPassed = AllConditionsPassed || ConditionPassed;
                                UE_LOG(LogTemp, Log, 
                                    TEXT("LookupVariableSub; for variablename %s's condition state var %s, 'or' chain gives allconditions passed as %d"), 
                                    *VariableName, 
                                    *StateVarName, 
                                    AllConditionsPassed
                                );
                            }
                        }
                        else
                        {
                            AllConditionsPassed = ConditionPassed;
                            UE_LOG(LogTemp, Log, 
                                TEXT("LookupVariableSub; for variablename %s's condition state var %s, no bool chain operator so allconditions passed is %d"), 
                                *VariableName, 
                                *StateVarName, 
                                AllConditionsPassed
                            );
                        }
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("LookupVariableSub; failed to coerce a Condition FJsonValue to JSON object."));
                    }
                }

                FString SubstitutionString;
                if (AllConditionsPassed)
                {
                    SubstitutionString = TopLevelVarEntry->GetStringField(KEY_PASS_SUBSTITUTION);
                }
                else
                {
                    SubstitutionString = TopLevelVarEntry->GetStringField(KEY_FAIL_SUBSTITUTION);
                }

                // run pass/fail sub string through StringDoctor() for recursive var sub
                StringDoctor(SubstitutionString);
                return SubstitutionString;
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("LookupVariableSub; failed to *deserialize* json, sure"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("LookupVariableSub; failed to read in text sub json at %s"), *TextVTableJSONPath);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("LookupVariableSub; gamestate came up null"));
    }
      
    return TEXT("Oops Error Substitution");
}