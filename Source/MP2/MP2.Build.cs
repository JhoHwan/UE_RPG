// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class MP2 : ModuleRules
{
	public MP2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[] { "MP2" });

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayAbilities", "GameplayTags", "GameplayTasks", "Navmesh",
			"NavigationSystem",});

		PrivateDependencyModuleNames.AddRange(new string[] { "HTTP", "Json", "Slate", "SlateCore", "GameNet", "ProtobufCore", "DeveloperSettings" });
    }
}
