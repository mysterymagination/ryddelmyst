// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class RyddelmystTarget : TargetRules
{
	public RyddelmystTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		ExtraModuleNames.Add("Ryddelmyst");
	}
}