// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PaperSprite.h"
#include "TextDisplayWidget.generated.h"

/**
 * A simple text display template containing text data field and accessors; the actual UI aspect is intended to be fleshed out in the Widget Blueprint subclass.
 */
UCLASS()
class RYDDELMYST_API UTextDisplayWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	FText DisplayText;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	FString PortraitName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	UPaperSprite* Portrait;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	bool IsDialogueDisplayed = false;

public:
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	const FText& GetText() const { return DisplayText;  };

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SetText(const FText& Text) { DisplayText = Text; };

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	const FString& GetPortraitName() const { return PortraitName;  };

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SetPortraitName(const FString& String) { PortraitName = String; };

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	const UPaperSprite* GetPortrait() const { return Portrait; };

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SetPortrait(UPaperSprite* Image) { Portrait = Image; };
};
