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
#include "Components/GridSlot.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "TextDisplayWidget.h"
#include "Kismet/GameplayStatics.h"
#include "LibraryBookWidget.h"
#include "Describable.h"
#include "RyddelmystGameState.h"
#include "Components/TextBlock.h"
#include "Layout/Margin.h"
#include "Components/HorizontalBoxSlot.h"

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

	static ConstructorHelpers::FClassFinder<UUserWidget> CreditsWidgetObj(TEXT("/Game/Ryddelmyst_Assets/UI/BP_CreditsRoll"));
	CreditsWidgetClass = CreditsWidgetObj.Class;
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
				InventorySelectionOverlay = Cast<UHorizontalBox>(InventorySelectionOverlayWidget);

				if (InventorySelectionTexture)
				{
					InventorySelectionIcon = StatusWidget->WidgetTree->ConstructWidget<UImage>();
					FSlateBrush ModBrush = InventorySelectionIcon->Brush;
					ModBrush.SetImageSize(FVector2D(128, 128));
					ModBrush.SetResourceObject(InventorySelectionTexture);
					InventorySelectionIcon->SetBrush(ModBrush);
				}

				ShowDialogue(nullptr, FText::FromString(
					"What the?! Where am I? Hm, pretty cozy actually. I like it! "
					"For some reason I feel compelled to tell myself, aloud, that "
					"I can press the 'E Button' to interact with objects and close "
					"out or advance 'UI Elements' including my own 'speech bubble'. "
					"I can even pick some things up! Oooookay... that shouldn't be surprising. "
					"Ooh wait here comes another -- I can also press the 'Q Button' "
					"to pause and review my gathered 'quest logs'. Girl-o, my intrusive "
					"thoughts are getting intricate! ALso, what am I doing standing on a table? AGAIN?"
					)
				);
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
	FSlateBrush ModBrush = IconWidget->Brush;
    ModBrush.SetImageSize(FVector2D(128, 128));
	ModBrush.SetResourceObject(tex);
    IconWidget->SetBrush(ModBrush);
	EquipmentPanel->AddChildToHorizontalBox(IconWidget);
}

void ARyddelmystHUD::AddItemIcon(class UTexture2D* tex)
{
	UImage* IconWidget = StatusWidget->WidgetTree->ConstructWidget<UImage>();
	IconWidget->SetDesiredSizeOverride(FVector2D(128, 128)); // doesn't work, but recommend approach smdh
	// IconWidget->SetBrushSize(FVector2D(128, 128)); // compiler complains about deprecation and as of 5.4 it doesn't work anymore.
	FSlateBrush ModBrush = IconWidget->Brush;
    ModBrush.SetImageSize(FVector2D(128, 128));
	ModBrush.SetResourceObject(tex);
    IconWidget->SetBrush(ModBrush);
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
		auto* slot = InventorySelectionOverlay->AddChildToHorizontalBox(InventorySelectionIcon);
		slot->SetPadding(FMargin(idx*128,0,0,0));
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

bool ARyddelmystHUD::ShowConversation(UUserWidget* ConvoContent)
{
	if (!ConvoContent->IsInViewport())
	{
		ConvoContent->AddToViewport();
		return true;
	}
	else 
	{
		UE_LOG(LogTemp, Error, TEXT("ShowConvo; convo widget already in viewport"));
		return false;
	}
}

bool ARyddelmystHUD::ExitConversation(UUserWidget* ConvoContent)
{
	if (ConvoContent->IsInViewport())
	{
		ConvoContent->RemoveFromParent();
		return true;
	}
	else 
	{
		UE_LOG(LogTemp, Error, TEXT("ExitConvo; convo widget not in viewport"));
		return false;
	}
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

void ARyddelmystHUD::ExitPauseMenu()
{
	if (PauseMenuWidget)
	{
		if (PauseMenuWidget->IsInViewport())
		{
			PauseMenuWidget->RemoveFromParent();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ExitPauseMenu; pause menu widget not created"));
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
			LibraryWidget->SetGameState(GetWorld()->GetGameState<ARyddelmystGameState>());
			LibraryWidget->ReshelveBooks();
			LibraryWidget->PopulateUnshelved();
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

void ARyddelmystHUD::RollCredits(const FString& EndingContext)
{
	UE_LOG(LogTemp, Warning, TEXT("RollCredits"));
	FString ImagePath = TEXT("/Game/Ryddelmyst_Assets/Textures/");
	FString ImageName;
	FString Ending;
	if (EndingContext == ARyddelmystGameState::STATE_CLUE_ENDING_PRACTICAL_PAWN)
	{
		Ending = "Ending 1 of 5: Practical Pawn";
		ImageName = "practical_pawn.practical_pawn";
	}
	else if (EndingContext == ARyddelmystGameState::STATE_CLUE_ENDING_CRAVING_QUEEN_HOMEWARD)
	{
		Ending = "Ending 2 of 5: Craving Queen, Homeward";
		ImageName = "craving_queen_homeward.craving_queen_homeward";
	}
	else if (EndingContext == ARyddelmystGameState::STATE_CLUE_ENDING_CRAVING_QUEEN_HEAVENWARD)
	{
		Ending = "Ending 3 of 5: Craving Queen, Ascension";
		ImageName = "craving_queen_ascension.craving_queen_ascension";
	}
	else if (EndingContext == ARyddelmystGameState::STATE_CLUE_ENDING_AMOROUS_ANGEL_HOMEWARD)
	{
		Ending = "Ending 4 of 5: Amorous Angel, Homeward";
		ImageName = "amorous_angel_homeward.amorous_angel_homeward";
	}
	else if (EndingContext == ARyddelmystGameState::STATE_CLUE_ENDING_AMOROUS_ANGEL_HEAVENWARD)
	{
		Ending = "Ending 5 of 5: Amorous Angel, Ascension";
		ImageName = "amorous_angel_ascension3.amorous_angel_ascension3";
	}
	else if (EndingContext == ARyddelmystGameState::STATE_CLUE_ENDING_DEAD)
	{
		Ending = "Ending -1 of 5: Death";
		ImageName = "blizzardskull.blizzardskull";
	}
	else
	{
		Ending = "Ending ? of 5: Wtf";
	}
	UE_LOG(LogTemp, Warning, TEXT("RollCredits; ending is %s from context %s"), *Ending, *EndingContext);
	FString Credits;
	FString CreditsPath = FPaths::ProjectContentDir().Append(TEXT("Ryddelmyst_Assets/Text/PrettyCredits.txt"));
	FFileHelper::LoadFileToString(Credits, *CreditsPath);
	UE_LOG(LogTemp, Warning, TEXT("RollCredits; credits says %s"), *Credits);
	auto* BackgroundImage = LoadObject<UTexture2D>(nullptr, *ImagePath.Append(ImageName), nullptr, LOAD_None, nullptr);
	auto* CreditsWidget = CreateWidget<UUserWidget>(GetWorld(), CreditsWidgetClass);
	auto* CreditsText = CreditsWidget->WidgetTree->FindWidget<UTextBlock>(FName("CreditsTextBlock"));
	UE_LOG(LogTemp, Warning, TEXT("RollCredits; creditswidget says %p and creditstext says %p"), CreditsWidget, CreditsText);
	CreditsText->SetText(FText::FromString(Credits + TEXT("\n") + Ending));
	auto* BackgroundImageWidget = CreditsWidget->WidgetTree->FindWidget<UImage>(FName("Background"));
	BackgroundImageWidget->SetBrushFromTexture(BackgroundImage);
	CreditsWidget->AddToViewport();
}
