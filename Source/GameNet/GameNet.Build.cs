using UnrealBuildTool;
 
public class GameNet : ModuleRules
{
	public GameNet(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine"});
		PrivateDependencyModuleNames.AddRange(new string[] { "ProtobufCore", "Sockets", "Networking" });
 
		PublicIncludePaths.AddRange(new string[] { "GameNet/Public"});
		PrivateIncludePaths.AddRange(new string[] { "GameNet/Private"});
	}
}