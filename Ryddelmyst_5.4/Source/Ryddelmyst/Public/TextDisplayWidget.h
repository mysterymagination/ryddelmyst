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

	// todo: implement in BP for simplicity to cover convo in quest log double muzak edge case
	/**
	 * Spawn an AudioComponent and play the audio asset at the given path. The
	 * audio will be deactivated when the widget is destroyed.
	 * @param AudioAssetPath game package root relative path to the audio we want to play.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Dialogue")
	void PlayAudio(const FString& AudioAssetPath);
	void PlayAudio_Implementation(const FString& AudioAssetPath) { UE_LOG(LogTemp, Error, TEXT("PlayAudio; empty default impl. Please implement in BP.")) };
};
