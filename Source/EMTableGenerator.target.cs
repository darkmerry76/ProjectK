using UnrealBuildTool;
using System.Collections.Generic;

public class EMTableGeneratorTarget : TargetRules
{
	public EMTableGeneratorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Program;
		LinkType = TargetLinkType.Modular;
		LaunchModuleName = "EMTableGenerator";
		// 테이블제네레이터 빌드 할때 반드시 주석처리
		BuildEnvironment = TargetBuildEnvironment.Shared;
		bCompileAgainstEngine = false;
		bBuildDeveloperTools = false;
		bIsBuildingConsoleApplication = true;
	}
}
