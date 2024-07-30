// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RyddelmystHUD.generated.h"

UCLASS()
class ARyddelmystHUD : public AHUD
{
	GENERATED_BODY()

public:
	ARyddelmystHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;
	virtual void BeginPlay() override;
	class UUserWidget* GetStatusWidget();

	/**
	 * Called via input to Scroll Up event when dialogue scrolling is active; a dialogue is a minor comment or observation made by the player character that appears in the StatusWidget and does not block gameplay.
	 */
	UFUNCTION()
	void ScrollDialogueUp();

	/**
	 * Called via input to Scroll Down event when dialogue scrolling is active; a dialogue is a minor comment or observation made by the player character that appears in the StatusWidget and does not block gameplay.
	 */
	UFUNCTION()
	void ScrollDialogueDown();

	/**
	 * Called via input to Scroll Up event when text scrolling is active; a text display is a larger lore drop in the TextWidget that takes up the main player viewport and pauses the game.
	 */
	UFUNCTION()
	void ScrollTextUp();

	/**
	 * Called via input to Scroll Down event when text scrolling is active; a text display is a larger lore drop in the TextWidget that takes up the main player viewport and pauses the game.
	 */
	UFUNCTION()
	void ScrollTextDown();

	/**
	 * @return true if the dialogue box is displayed in the status widget, false otherwise
	 */
	UFUNCTION()
	bool IsDialogueActive();

	/**
	 * @return true if the TextWidget exists and is added to the viewport, false otherwise
	 */
	UFUNCTION()
	bool IsTextActive();

	/**
	 * Adds a UImage icon widget to the InventoryPanel, using the input texture for the image brush 
	 * @param tex the texture representing the item
	 */
	UFUNCTION()
	void AddItemIcon(class UTexture2D* tex);

	/**
	 * Adds a UImage icon widget to the EquipmentPanel, using the input texture for the image brush
	 * @param tex the texture representing the item
	 */
	UFUNCTION()
	void AddEquipIcon(class UTexture2D* tex);

	/**
	 * Removes the UImage icon widget at the given child index from the InventoryPanel
	 * @param idx the index of the item icon to remove
	 */
	UFUNCTION()
	void RemoveItemIcon(uint8 idx);

	/**
	 * Moves the InventorySelectionIcon to the column of InventorySelectionOverlay at the given index
	 * @param idx the index of the item being selected
	 */
	UFUNCTION()
	void SelectItem(uint8 idx);

	/**
	 * Removes the InventorySelectionIcon entirely
	 */
	UFUNCTION()
	void ClearItemSelection();

	/**
	 * @brief Attempts to show the dialogue box, if one is not already showing
	 * @param Portrait the character portrait to show beside the dialogue text
	 * @param Text the dialogue text
	 * @return true if a dialogue was shown, false otherwise 
	 * 
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool ShowDialogue(UPaperSprite* Portrait, const FText& Text);

	/**
	 * @brief Attempts to hide the dialogue box, if one is showing
	 * @return true if a showing dialogue was hidden, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool HideDialogue();

	/**
	 * @brief Attempts to show text in the mostly full screen text box, if one is not already showing
	 * @param Text the text to show over most of the screen
	 * @return true if text was shown, false otherwise 
	 * 
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool ShowText(const FText& Text);

	/**
	 * @brief Attempts to hide the mostly full screen text box, if one is showing
	 * @return true if a showing text was hidden, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool HideText();

	/**
	 * @brief hides all parts of the HUD
	 * 
	 */
	UFUNCTION(BlueprintCallable, Category = "GameState")
	void HideRyddelmystHUD();

	/**
	 * @brief shows only the crucial parts of the HUD (i.e. player status)
	 * 
	 */
	UFUNCTION(BlueprintCallable, Category = "GameState")
	void ShowRyddelmystHUD();

	UFUNCTION(BlueprintCallable, Category = "GameState")
	void ShowStatus();

	UFUNCTION(BlueprintCallable, Category = "GameState")
	void HideStatus();

	UFUNCTION(BlueprintCallable, Category = "GameState")
	void ShowPauseMenu();

	UFUNCTION(BlueprintCallable, Category = "GameState")
	void ShowGameOverMenu();

private:
	/** Crosshair asset pointer */
	UPROPERTY()
	class UTexture2D* CrosshairTex;

	UPROPERTY(EditAnywhere, Category = "RPG")
	TSubclassOf<class UTextDisplayWidget> StatusWidgetClass;

	UPROPERTY()
	class UTextDisplayWidget* StatusWidget;

	UPROPERTY(EditAnywhere, Category = "GameState")
	TSubclassOf<class UUserWidget> PauseMenuWidgetClass;

	UPROPERTY(EditAnywhere, Category = "GameState")
	TSubclassOf<class UUserWidget> GameOverMenuWidgetClass;

	UPROPERTY()
	class UUserWidget* PauseMenuWidget;

	UPROPERTY()
	class UUserWidget* GameOverMenuWidget;

	UPROPERTY()
	class UTextDisplayWidget* TextWidget;

	/** Handle to the InventoryPanel UI widget in the HUD */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	class UHorizontalBox* InventoryPanel;

	/** Handle to the EquipmentPanel UI widget in the HUD */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	class UHorizontalBox* EquipmentPanel;

	/** Handle to the InventorySelectionOverlay UI widget in the HUD */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	class UGridPanel* InventorySelectionOverlay;

	/** Icon used to indicate selection in the InventorySelectionOverlay UI widget in the HUD */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	class UImage* InventorySelectionIcon;

	/** Texture used to fill the InventorySelectionIcon */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	class UTexture2D* InventorySelectionTexture;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TSubclassOf<class UTextDisplayWidget> TextWidgetClass;
};

