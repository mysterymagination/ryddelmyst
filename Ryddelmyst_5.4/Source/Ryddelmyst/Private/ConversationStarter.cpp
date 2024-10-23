// Fill out your copyright notice in the Description page of Project Settings.


#include "ConversationStarter.h"

UUserWidget* UConversationStarter::ParseConversationScript(const FString& Script)
{
    // todo: parse script json into UI elements added to a wrapper slate widget; for simplicity and prettyness I guess the best approach would be to create a UI asset in the editor that acts as a scrollable container and then add generated elements such as images, text, and buttons from the parsing.
    // todo: when we parse out text strings from the 'lines' json elements, let's use FText and format args this time instead of our eliptical wheel version
    //  that doesn't support localization. NSLOCTEXT() is the macro you want, with namespace, key, and translatable string. Making the character names the namespace might be handy?
    return nullptr;
}