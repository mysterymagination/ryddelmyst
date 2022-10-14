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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dialogue")
	FText DisplayText;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dialogue")
	UPaperSprite* Portrait;

public:
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	const FText& GetText() const { return DisplayText;  };

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SetText(const FText& Text) { DisplayText = Text; };

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	const UPaperSprite* GetPortrait() const { return Portrait; };

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SetPortrait(UPaperSprite* Image) { Portrait = Image; };
};
