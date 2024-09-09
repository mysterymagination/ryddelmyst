// Copyright Jeff Creswell 2024. Subject to the GPL v3.0 license.


#include "AssetUtils.h"
#include "Engine/ObjectLibrary.h"

TArray<FString> AssetUtils::ListFilesUnderFolder(const FString& Path)
{
    TArray<FString> Files;
    UE_LOG(LogTemp, Log, TEXT("Finding files under %s"), *Path);
    IFileManager::Get().FindFiles(Files, *Path, true, false);
    return Files;
}

FString AssetUtils::ChooseRandomAsset(const FString& Path)
{
    TArray<FString> Files = ListFilesUnderFolder(Path);
    int index = FMath::RandRange(0, Files.Num()-1);
    return Files[index];
}

FString AssetUtils::ChooseRandomLadyExclamationAsset()
{
    TArray<FString> LadyFiles {
        TEXT("/Game/Ryddelmyst_Assets/Audio/VO/Maya/LadyExclamations/Female_Ouch.Female_Ouch"),
        TEXT("/Game/Ryddelmyst_Assets/Audio/VO/Maya/LadyExclamations/Female_Pain_with_Ugh.Female_Pain_with_Ugh"),
        TEXT("/Game/Ryddelmyst_Assets/Audio/VO/Maya/LadyExclamations/Female_Good_Ouch.Female_Good_Ouch"),
        TEXT("/Game/Ryddelmyst_Assets/Audio/VO/Maya/LadyExclamations/Female_Good_Oof.Female_Good_Oof"),
        TEXT("/Game/Ryddelmyst_Assets/Audio/VO/Maya/LadyExclamations/Female_Frustration.Female_Frustration"),
        TEXT("/Game/Ryddelmyst_Assets/Audio/VO/Maya/LadyExclamations/female_exasperated_yell.female_exasperated_yell"),
        TEXT("/Game/Ryddelmyst_Assets/Audio/VO/Maya/LadyExclamations/Female_Disgust.Female_Disgust"),
        TEXT("/Game/Ryddelmyst_Assets/Audio/VO/Maya/LadyExclamations/Female_Cough_Gasp.Female_Cough_Gasp")
    };
    int index = FMath::RandRange(0, LadyFiles.Num()-1);
    FString LadyFile = LadyFiles[index];
    UE_LOG(LogTemp, Log, TEXT("ChooseRandomLadyExclamationAsset; chosen file is %s"), *LadyFile);
    return LadyFile;
}