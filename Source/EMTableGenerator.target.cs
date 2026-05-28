using UnrealBuildTool;
using System.Collections.Generic;

public class EMTableGeneratorTarget : TargetRules
{
	public EMTableGeneratorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Program;
		LinkType = TargetLinkType.Modular;
		LaunchModuleName = "EMTableGenerator";
		BuildEnvironment = TargetBuildEnvironment.Shared;
		bCompileAgainstEngine = false;
		bBuildDeveloperTools = false;
		bIsBuildingConsoleApplication = true;
	}
}
