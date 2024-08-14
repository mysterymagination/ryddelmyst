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