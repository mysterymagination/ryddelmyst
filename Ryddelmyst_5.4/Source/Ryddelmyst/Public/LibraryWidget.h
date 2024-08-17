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

public:
	/**
	 * @brief Pulls a book from the appropriate pool for a book widget to display.
	 * @param Category the genre this book should be shelved into. 
	 */
	UFUNCTION(BlueprintCallable, Category = "Lore")
	FLibraryBookData PullUnshelved(ELibraryCat Category);
	/**
	 * @brief adds FLibraryBookData to the BookBank mapping
	 */
	UFUNCTION(BlueprintCallable, Category = "Lore")
	void AddBook(const FLibraryBookData& Data);

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
	
};
