// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Ryddelmyst : ModuleRules
{
	public Ryddelmyst(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG", "Slate", "SlateCore", "Paper2D", "Niagara" });
	}
}
