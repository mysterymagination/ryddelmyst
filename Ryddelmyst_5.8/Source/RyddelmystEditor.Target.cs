// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class RyddelmystEditorTarget : TargetRules
{
	public RyddelmystEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		ExtraModuleNames.Add("Ryddelmyst");
	}
}
