// Copyright Epic Games, Inc. All Rights Reserved.

#include "RyddelmystHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/Image.h"
#include "TextDisplayWidget.h"

ARyddelmystHUD::ARyddelmystHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;

	static ConstructorHelpers::FClassFinder<UUserWidget> StatusWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_Status"));
	StatusWidgetClass = StatusWidgetObj.Class;
	
	static ConstructorHelpers::FClassFinder<UUserWidget> PauseMenuWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_PauseMenu"));
	PauseMenuWidgetClass = PauseMenuWidgetObj.Class;

	static ConstructorHelpers::FObjectFinder<UTexture2D> SelectionTexObj(TEXT("/Game/Ryddelmyst_Assets/Textures/SelectionHighlight"));
	InventorySelectionTexture = SelectionTexObj.Object;
}


void ARyddelmystHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition( (Center.X),
										   (Center.Y + 20.0f));

	// draw the crosshair
	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( TileItem );
}

void ARyddelmystHUD::BeginPlay()
{
	Super::BeginPlay();
	FString MapName = GetWorld()->GetMapName();
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay; map name is %s"), *MapName);
	if(!GetWorld()->GetMapName().Contains("splash", ESearchCase::Type::IgnoreCase))
	{
		if (StatusWidgetClass)
		{
			StatusWidget = CreateWidget<UUserWidget>(GetWorld(), StatusWidgetClass);
			UE_LOG(LogTemp, Warning, TEXT("BeginPlay; status widget says %p"), StatusWidget)
			if (StatusWidget)
			{
				StatusWidget->AddToViewport();
				// UI setup
				UWidget* InventoryPanelWidget = StatusWidget->WidgetTree->FindWidget(FName("InventoryPanel"));
				InventoryPanel = Cast<UHorizontalBox>(InventoryPanelWidget);
				UWidget* InventorySelectionOverlayWidget = StatusWidget->WidgetTree->FindWidget(FName("InventorySelectionOverlay"));
				InventorySelectionOverlay = Cast<UGridPanel>(InventorySelectionOverlayWidget);

				if (InventorySelectionTexture)
				{
					InventorySelectionIcon = StatusWidget->WidgetTree->ConstructWidget<UImage>();
					InventorySelectionIcon->SetBrushSize(FVector2D(FIntPoint(128, 128)));
					InventorySelectionIcon->SetBrushFromTexture(InventorySelectionTexture, false);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("BeginPlay; no status widget class set"));
		}

		if (TextWidgetClass)
		{
			TextWidget = CreateWidget<UTextDisplayWidget>(GetWorld(), TextWidgetClass);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("BeginPlay; no text widget class set"));
		}

		if (PauseMenuWidgetClass)
		{
			PauseMenuWidget = CreateWidget<UUserWidget>(GetWorld(), PauseMenuWidgetClass);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("BeginPlay; no pause menu widget class set"));
		}
	}
}

UUserWidget* ARyddelmystHUD::GetStatusWidget()
{
	return StatusWidget;
}

void ARyddelmystHUD::AddItemIcon(class UTexture2D* tex)
{
	UImage* IconWidget = StatusWidget->WidgetTree->ConstructWidget<UImage>();
	IconWidget->SetBrushSize(FVector2D(FIntPoint(128, 128)));
	IconWidget->SetBrushFromTexture(tex, false);
	InventoryPanel->AddChildToHorizontalBox(IconWidget);
}

void ARyddelmystHUD::RemoveItemIcon(uint8 idx)
{
	InventoryPanel->RemoveChildAt(idx);
}

void ARyddelmystHUD::SelectItem(uint8 idx)
{
	// clear selection overlay
	InventorySelectionOverlay->ClearChildren();
	// add selection overlay image
	if (InventorySelectionIcon)
	{
		UE_LOG(LogTemp, Warning, TEXT("SelectItem; idx is %u"), idx);
		// todo: htf do you properly use this fella as a grid?!  Adding children to successive indices just lands us in what I would expect to be column 0 and there's no way to set the column and row dimensions, nor number of columns and rows.  GridSlots let you mess with col and row stuff, but that would suggest I need to populate the grid with empty cells before I can work with it as, y'know, a grid?
		UGridSlot* slot = InventorySelectionOverlay->AddChildToGrid(InventorySelectionIcon, 0, 0);
		// todo: look up the image width programmatically
		slot->SetNudge(FVector2D(idx*128, 0.f));
		slot->SynchronizeProperties();
	}
}

void ARyddelmystHUD::ClearItemSelection()
{
	InventorySelectionOverlay->ClearChildren();
}

void ARyddelmystHUD::ShowDialogue(const FText& Text)
{
	if (TextWidget)
	{

		TextWidget->SetText(Text);
		if (!TextWidget->IsInViewport())
		{
			TextWidget->AddToViewport();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ShowDialogue; text widget not created yet"));
	}
}

void ARyddelmystHUD::HideDialogue()
{
	if (TextWidget)
	{
		if (TextWidget->IsInViewport())
		{
			TextWidget->RemoveFromViewport();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HideDialogue; text widget not created yet"));
	}
}

void ARyddelmystHUD::HideStatus()
{
	if (StatusWidget)
	{
		if (StatusWidget->IsInViewport())
		{
			StatusWidget->RemoveFromViewport();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HideStatus; status widget not created yet"));
	}
}

void ARyddelmystHUD::ShowStatus()
{
	if (StatusWidget)
	{
		if (!StatusWidget->IsInViewport())
		{
			StatusWidget->AddToViewport();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ShowStatus; status widget not created yet"));
	}
}

void ARyddelmystHUD::HideRyddelmystHUD()
{
	HideDialogue();
	HideStatus();
	if(Canvas)
	{
		Canvas->Reset();
	}
}

void ARyddelmystHUD::ShowRyddelmystHUD()
{
	ShowStatus();
}

void ARyddelmystHUD::ShowPauseMenu()
{
	if (PauseMenuWidget)
	{
		if (!PauseMenuWidget->IsInViewport())
		{
			PauseMenuWidget->AddToViewport();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ShowPauseMenu; pause menu widget not created"));
	}
}