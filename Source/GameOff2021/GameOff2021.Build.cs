// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GameOff2021 : ModuleRules
{
	public GameOff2021(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
