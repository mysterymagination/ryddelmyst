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

public:
	/**
	 * @brief collection of book data which the library will need to create LibraryBookWidgets in the UI; it is organized as a 
	 * genre key to book data array so we can easily gen up widgets for each book in each category.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lore")
	TMap<ELibraryCat, FLibraryBookShelf> BookBank;

public:
	/**
	 * @brief adds FLibraryBookData to the BookBank mapping
	 */
	UFUNCTION(BlueprintCallable, Category = "Lore")
	void AddBook(const FLibraryBookData& Data);

	/**
	 * @brief iterates over the BookBank map to create a UI filled with interactable ULibraryBookWidgets
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Lore")
	void ShelveBooks();
	void ShelveBooks_Implementation() {};
	
};
