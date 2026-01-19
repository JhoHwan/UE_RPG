using System.IO;
using UnrealBuildTool;

public class ProtobufCore : ModuleRules
{
    public ProtobufCore(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;

        string ProtobufPath = Path.Combine(ModuleDirectory);

        PublicSystemIncludePaths.Add(Path.Combine(ProtobufPath, "Include"));
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicAdditionalLibraries.Add(Path.Combine(ProtobufPath, "Lib", "libprotobuf.lib"));
        }
        PublicDefinitions.Add("GOOGLE_PROTUBUF_NO_RTTI=1");
    }
}