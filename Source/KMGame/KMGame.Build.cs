// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class KMGame : ModuleRules
{
	public KMGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EMCore",
			"EMCoreGame",
			"EMTableCore",
			"EMDataTable",
			"UMG",
			"SlateCore",
			"Slate",
			"AIModule",
			"GameplayTasks",
			"ApplicationCore",
			"DeveloperSettings",
			"AnimGraphRuntime",
			"EnhancedInput",
			"GameplayTags",
			"EMCurveWarping",
			"MotionWarping",
			"Niagara",
			"EMMartialArts",
			"TemplateSequence"
		});
		
		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			PublicDependencyModuleNames.Add("Launch");
		}
		
		PublicIncludePaths.Add("KMGame");
	}
}
