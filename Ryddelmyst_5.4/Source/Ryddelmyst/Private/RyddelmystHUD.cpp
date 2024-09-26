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
#include "LibraryBookWidget.h"
#include "Describable.h"
#include "RyddelmystGameState.h"

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

	static ConstructorHelpers::FClassFinder<UUserWidget> LibraryWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_Library"));
	LibraryWidgetClass = LibraryWidgetObj.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> LibraryBookWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_LibraryBook"));
	LibraryBookWidgetClass = LibraryBookWidgetObj.Class;

	static ConstructorHelpers::FObjectFinder<UTexture2D> SelectionTexObj(TEXT("/Game/Ryddelmyst_Assets/Textures/SelectionHighlight"));
	InventorySelectionTexture = SelectionTexObj.Object;

	static ConstructorHelpers::FClassFinder<UUserWidget> TextWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_TextDisplay"));
	TextWidgetClass = TextWidgetObj.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> BookTextWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_BookTextDisplay"));
	BookTextWidgetClass = BookTextWidgetObj.Class;
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
					InventorySelectionIcon->SetDesiredSizeOverride(FVector2D(128, 128));
					InventorySelectionIcon->SetBrushFromTexture(InventorySelectionTexture, false);
				}

				ShowDialogue(nullptr, FText::FromString("What the?! Where am I? Hm, pretty cozy actually. I like it! For some reason I feel compelled to tell myself, aloud, that I can press the 'E Button' to interact with objects and close out or advance 'UI Elements' including my own 'speech bubble'. Oooookay... Ooh wait here comes another -- I can also press the 'Q Button' to pause and review my gathered 'quest logs'. Girl-o, my intrusive thoughts are getting intricate! ALso, what am I doing standing on a table? AGAIN?"));
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

		if (BookTextWidgetClass)
		{
			BookTextWidget = CreateWidget<UTextDisplayWidget>(GetWorld(), BookTextWidgetClass);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("BeginPlay; no book text widget class set"));
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

		if (LibraryWidgetClass)
		{
			LibraryWidget = CreateWidget<ULibraryWidget>(GetWorld(), LibraryWidgetClass);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("BeginPlay; no library widget class set"));
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
	IconWidget->SetDesiredSizeOverride(FVector2D(96, 96));
	EquipmentPanel->AddChildToHorizontalBox(IconWidget);
}

void ARyddelmystHUD::AddItemIcon(class UTexture2D* tex)
{
	UImage* IconWidget = StatusWidget->WidgetTree->ConstructWidget<UImage>();
	IconWidget->SetBrushFromTexture(tex, false);
	IconWidget->SetDesiredSizeOverride(FVector2D(128, 128));
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
			TextWidget->RemoveFromParent();
			return true;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HideText; text widget not created yet"));
	}
	return false;
}

bool ARyddelmystHUD::ShowBookText(const FText& Text)
{
	if (BookTextWidget)
	{
		BookTextWidget->SetText(Text);
		if (!BookTextWidget->IsInViewport())
		{
			BookTextWidget->AddToViewport();
			return true;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ShowDialogue; book text widget not created yet"));
	}
	return false;
}

bool ARyddelmystHUD::HideBookText()
{
	if (BookTextWidget)
	{
		if (BookTextWidget->IsInViewport())
		{
			BookTextWidget->RemoveFromParent();
			return true;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HideBookText; book text widget not created yet"));
	}
	return false;
}

bool ARyddelmystHUD::ShowConversation(const FString& JsonString)
{
	// todo: use convo parser to generate conversation UI from input JSON string; this should include an exit convo button or something at the bottom that removes the whole UI tree from the HUD.
	return false;
}

void ARyddelmystHUD::HideStatus()
{
	if (StatusWidget)
	{
		if (StatusWidget->IsInViewport())
		{
			StatusWidget->RemoveFromParent();
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

void ARyddelmystHUD::AddLore(const FLibraryBookData& Data)
{
	if (LibraryWidget)
	{
		// todo: so something weird -- for some reason we don't proc the BP constructor script at this point.
		//  Further, when the book is constructed at the point we show the library (new book instance each time),
		//  it doesn't have the Lore data we pass in here. That data exists just fine in refresh lore, so the CPP to BP
		//  pipeline is fine, but the lifecycle and copy-construct (?) is not. With Unreal being so weird about ctors for
		//  UObjects I don't know if we even could define a copy constructor for the book widget, much less if it would
		//  actually get called. I think the better thing to do is dodge the whole janky UI object lifecycle mess entirely
		//  by keeping a map of book data in memory and then having the library construct and populate all needed book widgets
		//  when it gets displayed. Since it's constructing new instances of the books at this point anyway, we might as well
		//  consider this the entry point for the actual book widget. 
		//ULibraryBookWidget* LibraryBook = CreateWidget<ULibraryBookWidget>(GetWorld(), LibraryBookWidgetClass);
		//LibraryBook->SetLore(Data);
		// todo: look up utexture2d from Data.CoverArtPath
		//LibraryBook->SetCoverArt(); // mrr... runtime asset loading. Nevermind, the book logo is good enough for demo!

		// add library book to library
		LibraryWidget->AddBook(Data);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BeginPlay; no library widget set"));
	}
}

FLibraryBookData ARyddelmystHUD::PullUnshelved(ELibraryCat Category)
{
	return LibraryWidget->PullUnshelved(Category);
}

bool ARyddelmystHUD::ShowLibrary()
{
	if (LibraryWidget)
	{
		if (!LibraryWidget->IsInViewport())
		{
			// order of operations here is important -- we need to take care of data mucking about before
			// actually displaying the library, which triggers its 'construction' script.
			LibraryWidget->ReshelveBooks();
			LibraryWidget->PopulateUnshelved();
			LibraryWidget->SetGameState(GetWorld()->GetGameState<ARyddelmystGameState>());
			LibraryWidget->AddToViewport();
			return true;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ShowLibrary; library widget not created yet"));
	}
	return false;
}

bool ARyddelmystHUD::HideLibrary()
{
	if (LibraryWidget)
	{
		if (LibraryWidget->IsInViewport())
		{
			LibraryWidget->RemoveFromParent();
			return true;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HideLibrary; Library widget not created yet"));
	}
	return false;
}
