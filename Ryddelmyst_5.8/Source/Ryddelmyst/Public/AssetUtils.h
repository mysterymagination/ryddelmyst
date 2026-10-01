// Copyright Jeff Creswell 2024. Subject to the GPL v3.0 license.

#pragma once

#include "CoreMinimal.h"

/**
 * Utility class for working with Assets at runtime
 */
class RYDDELMYST_API AssetUtils
{
public:
	/**
	 * @return a list of asset paths relative to the gameplay content root (or possibly the host filesystem root? idk, doesn't work)
	 */
	static TArray<FString> ListFilesUnderFolder(const FString& Path);
	/**
	 * @return a randomly selected asset file path from directly under the given folder path (does not recurse subdirs, and also doesn't work)
	 */
	static FString ChooseRandomAsset(const FString& Path);
	/**
	 * @return *sigh* hack because UObjectLibrary and IAssetRegistry docs are either missing or TOO LONG so whatever here's a randomly chosen fella from a hardcoded set. Leave me alone.
	 */
	static FString ChooseRandomLadyExclamationAsset();
};
