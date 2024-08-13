// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Texture2D.h"
#include "LibraryBookWidget.generated.h"

/**
 * A widget for displaying longform lore about the game world to the player. These widgets will appear as a combination of 'cover art' sprite icon 
 * and 'title' summary text in columnated lists delimited by category. The detailed contents will be displayed when the items is clicked.
 */
UCLASS()
class RYDDELMYST_API ULibraryBookWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lore")
	FText TitleText;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lore")
	FText ContentsText;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lore")
	UTexture2D* CoverArt;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lore")
	bool AreContentsDisplayed = false;

public:
	UFUNCTION(BlueprintCallable, Category = "Lore")
	const FText& GetContents() const { return ContentsText;  };

	UFUNCTION(BlueprintCallable, Category = "Lore")
	void SetContents(const FText& Text) { ContentsText = Text; };

	UFUNCTION(BlueprintCallable, Category = "Lore")
	const FText& GetTitle() const { return TitleText; };

	UFUNCTION(BlueprintCallable, Category = "Lore")
	void SetTitle(const FText& Text) { TitleText = Text; };

	UFUNCTION(BlueprintCallable, Category = "Lore")
	const UTexture2D* GetCoverArt() const { return CoverArt; };

	UFUNCTION(BlueprintCallable, Category = "Lore")
	void SetCoverArt(UTexture2D* Image) { CoverArt = Image; };
	
};
