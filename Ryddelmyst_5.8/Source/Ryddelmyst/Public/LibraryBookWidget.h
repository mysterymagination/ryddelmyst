// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Texture2D.h"
#include "LibraryBookWidget.generated.h"

UENUM(BlueprintType)
enum class ELibraryCat : uint8
{
	Conversation	UMETA(DisplayName = "Conversation"),
	Diary			UMETA(DisplayName = "Diary Entry"),
	Observation		UMETA(DisplayName = "Observation"),
	Count			UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(ELibraryCat, ELibraryCat::Count);

USTRUCT(BlueprintType)
struct FLibraryBookData
{
	GENERATED_BODY()
public:
	/**
	 * @brief Quick description of the object, suitable for library book lists.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lore")
	FText LocalizedTitle;
	/**
	 * @brief Long form details about the object, suitable for quest log entry.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lore")
	FText LocalizedLore;
	/**
	 * @brief string path relative to game assets where the desired library book cover art image resides.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lore")
	FString CoverArtPath;
	/**
	 * @brief string JSON driving the generation of a conversation UI.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lore")
	FString ConversationScript;
	/**
	 * @brief determines the category section this book fits under in the library.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lore")
	ELibraryCat Genre = ELibraryCat::Observation;
};

/**
 * A widget for displaying longform lore about the game world to the player. These widgets will appear as a combination of 'cover art' sprite icon 
 * and 'title' summary text in columnated lists delimited by category. The detailed contents will be displayed when the items is clicked.
 */
UCLASS()
class RYDDELMYST_API ULibraryBookWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	/**
	 * @brief Data for the quest log entry.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lore")
	FLibraryBookData Lore;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lore")
	bool AreContentsDisplayed = false;

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Lore")
	void RefreshLore();
	void RefreshLore_Implementation() {};
	
	UFUNCTION(BlueprintCallable, Category = "Lore")
	const FLibraryBookData& GetLore() const { return Lore;  };

	UFUNCTION(BlueprintCallable, Category = "Lore")
	void SetLore(const FLibraryBookData& Data) { Lore = Data; };
	
};
