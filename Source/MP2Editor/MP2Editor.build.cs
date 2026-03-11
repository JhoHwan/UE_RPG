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
			"ProceduralMeshComponent",  // 메쉬 생성용
			"NavigationSystem"          // 내비게이션 데이터 접근용
		});
 
		PublicIncludePaths.AddRange(new string[] {"MP2Editor/Public"});
		PrivateIncludePaths.AddRange(new string[] {"MP2Editor/Private"});
	}
}