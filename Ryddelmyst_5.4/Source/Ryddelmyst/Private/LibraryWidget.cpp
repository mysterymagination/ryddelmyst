// Copyright Jeff Creswell 2024. Subject to the GPL v3.0 license.


#include "LibraryWidget.h"

void ULibraryWidget::AddBook(const FLibraryBookData& Data)
{
    if (BookBank.Contains(Data.Genre))
    {
        // update case where we need to ensure the given genre book array does not contain an entry with identical information.
        bool Duplicate = false;
        for (auto Book : BookBank[Data.Genre].Books)
        {
            if (Data.Genre == ELibraryCat::Observation || Data.Genre == ELibraryCat::Diary)
            {
                if (Data.LocalizedLore.EqualToCaseIgnored(Book.LocalizedLore))
                {
                    Duplicate = true;
                }
            }
            else 
            {
                if (Data.ConversationScript.Equals(Book.ConversationScript, ESearchCase::IgnoreCase))
                {
                    Duplicate = true;
                }
            }
        }

        if (!Duplicate)
        {
            BookBank[Data.Genre].Books.Add(Data);
        }
    }
    else 
    {
        // simple insert case for novel genre.
        FLibraryBookShelf Shelf;
        Shelf.Books.Add(Data);
        BookBank.Add(Data.Genre, Shelf);
    }
}

void ULibraryWidget::BookDoctor(FLibraryBookData& Data)
{
    // todo: scan the book for string template vars and replace them based on current game state.
}

void ULibraryWidget::PopulateUnshelved()
{
    if (BookBank.Contains(ELibraryCat::Observation))
    {
        for (auto Book : BookBank[ELibraryCat::Observation].Books)
        {
            BookDoctor(Book);
        }
        UnshelvedObservations = BookBank[ELibraryCat::Observation].Books;
    }
    if (BookBank.Contains(ELibraryCat::Conversation))
    {
        for (auto Book : BookBank[ELibraryCat::Conversation].Books)
        {
            BookDoctor(Book);
        }
        UnshelvedConversations = BookBank[ELibraryCat::Conversation].Books;
    }
    if (BookBank.Contains(ELibraryCat::Diary))
    {
        for (auto Book : BookBank[ELibraryCat::Diary].Books)
        {
            BookDoctor(Book);
        }
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

void ULibraryWidget::BookDoctor(FLibraryBookData& Data)
{
    // todo: search for instances of ${} template vars in the Lore of the input data
    // todo: parse out the variable name inside the template syntax
    // todo: look up the variable name and query gamestate to see what the replacement text value should be
}