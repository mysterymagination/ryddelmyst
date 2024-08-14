// Copyright Jeff Creswell 2024. Subject to the GPL v3.0 license.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LibraryBookWidget.h"
#include "LibraryWidget.generated.h"

/**
 * A displayable UI collection of lore entry LibraryBookWidgets. Buncha cruncha books. Home. Love.
 * Intended to be implemented in BP; default CPP impl will be a nop.
 */
UCLASS()
class RYDDELMYST_API ULibraryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Lore")
	void AddBook(ULibraryBookWidget* Book);
	void AddBook_Implementation(ULibraryBookWidget* Book) {};
	
};
