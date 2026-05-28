// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class EMTableGenerator : ModuleRules
{
	public EMTableGenerator(ReadOnlyTargetRules Target) : base(Target)
	{
		//OptimizeCode = CodeOptimization.Never;
		
		PrivatePCHHeaderFile = "Private/EMTableGenerator.h";
			
		string enginePath = Path.GetFullPath(Target.RelativeEnginePath);
		
		PublicIncludePaths.Add(Path.Combine(enginePath, "Source", "Runtime", "Launch", "Public"));
		PrivateIncludePaths.Add(Path.Combine(enginePath, "Source", "Runtime", "Launch", "Private"));

        PrivateDependencyModuleNames.AddRange(new string[]
        {
	        "Core", "CoreUObject", "Projects", "ApplicationCore", "Json", "JsonUtilities", "zlib", "EMTableCore"
        });
        
        string thirdPartyPath = Path.Combine(ModuleDirectory, "../../ThirdParty");
        
        PublicIncludePaths.Add(Path.Combine(thirdPartyPath, "libxl/Include"));
        PublicIncludePaths.Add(Path.Combine(thirdPartyPath, "pugixml/Include"));

        PublicDefinitions.Add("PUGIXML_WCHAR_MODE=1");

        if (Target.Platform == UnrealTargetPlatform.Win64)
		{
            PublicAdditionalLibraries.Add(Path.Combine(thirdPartyPath, "libxl/lib/windows/x64/release", "libxl.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(thirdPartyPath, "pugixml/lib/windows/x64/release", "pugixml.lib"));
        }
	}
}
