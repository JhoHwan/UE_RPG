using UnrealBuildTool;
 
public class MP2Editor : ModuleRules
{
	public MP2Editor(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "UnrealEd",
			"Blutility",                // Editor Utility Widget용
			"UMG",                      // UI 시스템
		});
 
		PublicIncludePaths.AddRange(new string[] {"MP2Editor/Public"});
		PrivateIncludePaths.AddRange(new string[] {"MP2Editor/Private"});
		
		PrivateDependencyModuleNames.AddRange(new string[] 
		{
			"Json", 
			"JsonUtilities",
			"ProceduralMeshComponent", 
			"NavigationSystem", "MP2"
		});
	}
}