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
	 * Adds a UImage icon widget to the InventoryPanel, using the input texture for the image brush 
	 * @param tex the texture representing the item
	 */
	UFUNCTION()
	void AddItemIcon(class UTexture2D* tex);

	/**
	 * Moves the InventorySelectionIcon to the column of InventorySelectionOverlay at the given index
	 * @param idx the index of the item being selected
	 */
	UFUNCTION()
	void SelectItem(uint8 idx);

private:
	/** Crosshair asset pointer */
	UPROPERTY()
	class UTexture2D* CrosshairTex;

	UPROPERTY(EditAnywhere, Category = "Health")
	TSubclassOf<class UUserWidget> StatusWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Health")
	class UUserWidget* StatusWidget;

	/** Handle to the InventoryPanel UI widget in the HUD */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	class UHorizontalBox* InventoryPanel;

	/** Handle to the InventorySelectionOverlay UI widget in the HUD */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	class UGridPanel* InventorySelectionOverlay;

	/** Icon used to indicate selection in the InventorySelectionOverlay UI widget in the HUD */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	class UImage* InventorySelectionIcon;

	/** Texture used to fill the InventorySelectionIcon */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	class UTexture2D* InventorySelectionTexture;
};

