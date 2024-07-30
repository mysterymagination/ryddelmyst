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
#include "Components/ScrollBox.h"
#include "TextDisplayWidget.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

ARyddelmystHUD::ARyddelmystHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;

	static ConstructorHelpers::FClassFinder<UUserWidget> StatusWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_Status"));
	StatusWidgetClass = StatusWidgetObj.Class;
	
	static ConstructorHelpers::FClassFinder<UUserWidget> PauseMenuWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_PauseMenu"));
	PauseMenuWidgetClass = PauseMenuWidgetObj.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> GameOverMenuWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_GameOverMenu"));
	GameOverMenuWidgetClass = GameOverMenuWidgetObj.Class;

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
	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->GetResource(), FLinearColor::White);
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
			StatusWidget = CreateWidget<UTextDisplayWidget>(GetWorld(), StatusWidgetClass);
			UE_LOG(LogTemp, Warning, TEXT("BeginPlay; status widget says %p"), StatusWidget)
			if (StatusWidget)
			{
				StatusWidget->AddToViewport();
				// UI setup
				UWidget* EquipmentPanelWidget = StatusWidget->WidgetTree->FindWidget(FName("EquipmentPanel"));
				EquipmentPanel = Cast<UHorizontalBox>(EquipmentPanelWidget);
				UWidget* InventoryPanelWidget = StatusWidget->WidgetTree->FindWidget(FName("InventoryPanel"));
				InventoryPanel = Cast<UHorizontalBox>(InventoryPanelWidget);
				UWidget* InventorySelectionOverlayWidget = StatusWidget->WidgetTree->FindWidget(FName("InventorySelectionOverlay"));
				InventorySelectionOverlay = Cast<UGridPanel>(InventorySelectionOverlayWidget);

				if (InventorySelectionTexture)
				{
					InventorySelectionIcon = StatusWidget->WidgetTree->ConstructWidget<UImage>();
					InventorySelectionIcon->Brush.ImageSize = FVector2D(FIntPoint(128, 128));
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

		if (GameOverMenuWidgetClass)
		{
			GameOverMenuWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverMenuWidgetClass);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("BeginPlay; no game over menu widget class set"));
		}
	}
}

UUserWidget* ARyddelmystHUD::GetStatusWidget()
{
	return StatusWidget;
}

void ARyddelmystHUD::AddEquipIcon(class UTexture2D* tex)
{
	UImage* IconWidget = StatusWidget->WidgetTree->ConstructWidget<UImage>();
	IconWidget->SetBrushFromTexture(tex, false);
	IconWidget->Brush.ImageSize = FVector2D(FIntPoint(96, 96));
	EquipmentPanel->AddChildToHorizontalBox(IconWidget);
}

void ARyddelmystHUD::AddItemIcon(class UTexture2D* tex)
{
	UImage* IconWidget = StatusWidget->WidgetTree->ConstructWidget<UImage>();
	IconWidget->SetBrushFromTexture(tex, false);
	IconWidget->Brush.ImageSize = FVector2D(FIntPoint(128, 128));
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

void ARyddelmystHUD::ScrollDialogueUp()
{
	if (StatusWidget)
	{
		UScrollBox* DialogueScrollBox = StatusWidget->WidgetTree->FindWidget<UScrollBox>(FName("DialogueScrollBox"));
		if (DialogueScrollBox->GetScrollOffset() > 0.f)
		{
			DialogueScrollBox->SetScrollOffset(DialogueScrollBox->GetScrollOffset() - 50.f);
		}
	}
}

void ARyddelmystHUD::ScrollDialogueDown()
{
	if (StatusWidget)
	{
		UScrollBox* DialogueScrollBox = StatusWidget->WidgetTree->FindWidget<UScrollBox>(FName("DialogueScrollBox"));
		UE_LOG(LogTemp, Warning, TEXT("scrolling down; offsetofend says %f"), DialogueScrollBox->GetScrollOffsetOfEnd());
		if (DialogueScrollBox->GetScrollOffset() < DialogueScrollBox->GetScrollOffsetOfEnd())
		{
			DialogueScrollBox->SetScrollOffset(DialogueScrollBox->GetScrollOffset() + 50.f);
		}
	}
}

bool ARyddelmystHUD::IsDialogueActive()
{
	if (StatusWidget)
	{
		return StatusWidget->IsDialogueDisplayed;
	}
	else
	{
		return false;
	}
}

bool ARyddelmystHUD::ShowDialogue(UPaperSprite* Portrait, const FText& Text)
{
	if (StatusWidget)
	{
		StatusWidget->SetText(Text);
		if(Portrait)
		{
			StatusWidget->SetPortrait(Portrait);
		}
		StatusWidget->IsDialogueDisplayed = true;
		UScrollBox* DialogueScrollBox = StatusWidget->WidgetTree->FindWidget<UScrollBox>(FName("DialogueScrollBox"));
		DialogueScrollBox->ScrollToStart();
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ShowDialogue; status widget not created yet"));
	}
	return false;
}

bool ARyddelmystHUD::HideDialogue()
{
	if (StatusWidget)
	{
		if (StatusWidget->IsDialogueDisplayed)
		{
			StatusWidget->IsDialogueDisplayed = false;
			return true;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HideDialogue; status widget not created yet"));
	}
	return false;
}

bool ARyddelmystHUD::IsTextActive()
{
	if (TextWidget)
	{
		return TextWidget->IsInViewport();
	}
	else
	{
		return false;
	}
}

void ARyddelmystHUD::ScrollTextUp()
{
	if (TextWidget && TextWidget->IsInViewport())
	{
		UScrollBox* TextScrollBox = TextWidget->WidgetTree->FindWidget<UScrollBox>(FName("TextScrollBox"));
		if (TextScrollBox)
		{
			if (TextScrollBox->GetScrollOffset() > 0.f)
			{
				TextScrollBox->SetScrollOffset(TextScrollBox->GetScrollOffset() - 50.f);
			}
		}
		else 
		{
			UE_LOG(LogTemp, Error, TEXT("ScrollTextUp; textscrollbox widget not found"));
		}
	}
}

void ARyddelmystHUD::ScrollTextDown()
{
	if (TextWidget && TextWidget->IsInViewport())
	{
		UScrollBox* TextScrollBox = TextWidget->WidgetTree->FindWidget<UScrollBox>(FName("TextScrollBox"));
		if (TextScrollBox)
		{
			if (TextScrollBox->GetScrollOffset() < TextScrollBox->GetScrollOffsetOfEnd())
			{
				TextScrollBox->SetScrollOffset(TextScrollBox->GetScrollOffset() + 50.f);
			}
		}
		else 
		{
			UE_LOG(LogTemp, Error, TEXT("ScrollTextDown; textscrollbox widget not found"));
		}
	}
}

bool ARyddelmystHUD::ShowText(const FText& Text)
{
	if (TextWidget)
	{
		TextWidget->SetText(Text);
		if (!TextWidget->IsInViewport())
		{
			TextWidget->AddToViewport();
			return true;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ShowDialogue; text widget not created yet"));
	}
	return false;
}

bool ARyddelmystHUD::HideText()
{
	if (TextWidget)
	{
		if (TextWidget->IsInViewport())
		{
			TextWidget->RemoveFromViewport();
			return true;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HideDialogue; text widget not created yet"));
	}
	return false;
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

void ARyddelmystHUD::ShowGameOverMenu()
{
	if (GameOverMenuWidget)
	{
		if (!GameOverMenuWidget->IsInViewport())
		{
			GameOverMenuWidget->AddToViewport();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ShowGameOverMenu; game over menu widget not created"));
	}
}
