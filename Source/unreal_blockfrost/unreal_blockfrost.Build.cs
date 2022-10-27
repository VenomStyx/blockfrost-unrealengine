// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class unreal_blockfrost : ModuleRules
{
	public unreal_blockfrost(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
