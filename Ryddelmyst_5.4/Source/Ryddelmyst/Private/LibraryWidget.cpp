// Copyright Jeff Creswell 2024. Subject to the GPL v3.0 license.


#include "LibraryWidget.h"

void ULibraryWidget::AddBook(const FLibraryBookData& Data)
{
    if (BookBank.Contains(Data.Genre))
    {
        BookBank[Data.Genre].Books.Add(Data);
    }
    else 
    {
        FLibraryBookShelf Shelf;
        Shelf.Books.Add(Data);
        BookBank.Add(Data.Genre, Shelf);
    }
}

void ULibraryWidget::PopulateUnshelved()
{
    if (BookBank.Contains(ELibraryCat::Observation))
    {
        UnshelvedObservations = BookBank[ELibraryCat::Observation].Books;
    }
    else if (BookBank.Contains(ELibraryCat::Conversation))
    {
        UnshelvedConversations = BookBank[ELibraryCat::Conversation].Books;
    }
    else if (BookBank.Contains(ELibraryCat::Diary))
    {
        UnshelvedDiaries = BookBank[ELibraryCat::Diary].Books;
    }
}

FLibraryBookData ULibraryWidget::PullUnshelved(ELibraryCat Category)
{
    FLibraryBookData data;
    switch(Category)
	{
		case ELibraryCat::Observation:
            if (UnshelvedObservations.Num() >= 1)
            {
                data = UnshelvedObservations[UnshelvedObservations.Num()-1];
                UnshelvedObservations.RemoveAt(UnshelvedObservations.Num()-1);
            }
			break;
        case ELibraryCat::Conversation:
            if (UnshelvedConversations.Num() >= 1)
            {
			    data = UnshelvedConversations[UnshelvedConversations.Num()-1];
                UnshelvedConversations.RemoveAt(UnshelvedConversations.Num()-1);
            }
			break;
        case ELibraryCat::Diary:
			if (UnshelvedDiaries.Num() >= 1)
            {
                data = UnshelvedDiaries[UnshelvedDiaries.Num()-1];
                UnshelvedDiaries.RemoveAt(UnshelvedDiaries.Num()-1);
            }
			break;
        default:
            UE_LOG(LogTemp, Error, TEXT("Unrecognized book genre %i"), Category);
            break;
	}
    return data;
}