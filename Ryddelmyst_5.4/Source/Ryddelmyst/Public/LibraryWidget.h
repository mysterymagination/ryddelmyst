// Copyright Jeff Creswell 2024. Subject to the GPL v3.0 license.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LibraryBookWidget.h"
#include "LibraryWidget.generated.h"

USTRUCT(BlueprintType)
struct FLibraryBookShelf
{
	GENERATED_BODY()
public:
	/**
	 * @brief Array of books for this categorical shelf.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lore")
	TArray<FLibraryBookData> Books;
};
  
/**
 * A displayable UI collection of lore entry LibraryBookWidgets. Buncha cruncha books. Home. Love.
 * Intended to be implemented in BP; default CPP impl will be a nop.
 */
UCLASS()
class RYDDELMYST_API ULibraryWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	/**
	 * @brief pool of observation book data that library book widgets haven't picked up yet.
	 * They fight me at every turn setting their data before or after display except via ctor script or bound function;
	 * since we're not able to assign any genre etc. data to a given book widget we have no choice but to assume a blind
	 * bottom-up approach where we just pull in the next data chunk available in the relevant pool without much regard for sequence.
	 */
	UPROPERTY()
	TArray<FLibraryBookData> UnshelvedObservations;
	/**
	 * @brief pool of diary book data that library book widgets haven't picked up yet.
	 */
	UPROPERTY()
	TArray<FLibraryBookData> UnshelvedDiaries;
	/**
	 * @brief pool of conversation book data that library book widgets haven't picked up yet.
	 */
	UPROPERTY()
	TArray<FLibraryBookData> UnshelvedConversations;

public:
	/**
	 * @brief collection of book data which the library will need to create LibraryBookWidgets in the UI; it is organized as a 
	 * genre key to book data array so we can easily gen up widgets for each book in each category.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lore")
	TMap<ELibraryCat, FLibraryBookShelf> BookBank;
	/**
	 * @brief the game content dir relative path of the JSON file containing our text VTable for lore variable substitution e.g. "Oh sheep, it's ${Is_Monsterpus_Name_Discovered}"
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lore")
	FString TextVTableJSONPath;

	static const FString KEY_CONDITION_STATE_VARIABLE_NAME;
	static const FString KEY_CONDITION_STATE_VARIABLE_TYPE;
	static const FString KEY_CONDITION_BOOLEAN_CHAIN_OPERATOR;
	static const FString KEY_CONDITION_COMPARISON_OPERATOR;
	static const FString KEY_CONDITION_PASS_VALUE;
	static const FString KEY_PASS_SUBSTITUTION;
	static const FString KEY_FAIL_SUBSTITUTION;
	static const FString VALUE_CONDITION_STATE_VARIABLE_TYPE_BOOLEAN;
	static const FString VALUE_CONDITION_STATE_VARIABLE_TYPE_INTEGER;
	static const FString VALUE_CONDITION_COMPARISON_OPERATOR_EQ;
	static const FString VALUE_CONDITION_COMPARISON_OPERATOR_GTE;
	static const FString VALUE_CONDITION_BOOLEAN_CHAIN_OPERATOR_OR;
	static const FString VALUE_CONDITION_BOOLEAN_CHAIN_OPERATOR_AND;

public:
	/**
	 * @brief Pulls a book from the appropriate pool for a book widget to display.
	 * @param Category the genre this book should be shelved into. 
	 */
	UFUNCTION(BlueprintCallable, Category = "Lore")
	FLibraryBookData PullUnshelved(ELibraryCat Category);
	/**
	 * @brief adds FLibraryBookData to the BookBank mapping under the relevant genre.
	 * @param Data the library book data to add.
	 */
	UFUNCTION(BlueprintCallable, Category = "Lore")
	void AddBook(const FLibraryBookData& Data);
	/**
	 * @brief modifies a given book data struct, replacing string template vars with state-based values and reassembling the contents into localized text.
	 * @param Data the library book data to be doctored.
	 */
	UFUNCTION(BlueprintCallable, Category = "Lore")
	void BookDoctor(FLibraryBookData& Data);
	/**
	 * @brief modifies the input LoreString, replacing string template vars with state-based values.
	 * @param LoreString the string to be doctored.
	 */
	UFUNCTION(BlueprintCallable, Category = "Lore")
	void StringDoctor(FString& LoreString);

	/**
	 * @brief reads the content of BookBank into three emptyable pools of book data which the book widgets can blindly consume.
	 * Not bothering to properly set this up with N pools to support N genres because this is a stupid workaround to a widget quirk.
	 */
	UFUNCTION(BlueprintCallable, Category = "Lore")
	void PopulateUnshelved();

	/**
	 * @brief iterates over the BookBank map to create a UI filled with interactable ULibraryBookWidgets
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Lore")
	void ShelveBooks();
	void ShelveBooks_Implementation() {};

	/**
	 * @brief do whatever's necessary to get a clean population of BookBank books represented on the shelves.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Lore")
	void ReshelveBooks();
	void ReshelveBooks_Implementation() {};

private:
	UFUNCTION(Category = "Lore")
	FString LookupVariableSubstitution(const FString& VariableName);
	
};
