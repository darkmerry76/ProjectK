// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class KMEditor : ModuleRules
{
	public KMEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		OptimizeCode = CodeOptimization.Never;
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"Slate",
			"SlateCore",
			"KMGame",
			"UnrealEd",
			"EMCoreGame",
			"EMCoreEditor",
			"EMTableCore",
			"EMDataTable",
			"AnimGraph",
			"BlueprintGraph",
			"AnimationModifiers",
			"AnimationBlueprintLibrary",
			"ComponentVisualizers",
			"ToolWidgets",
			"EditorWidgets",
			"ToolMenus",
			"AssetDefinition",
			"EditorConfig",
			"TypedElementFramework",
			"TypedElementRuntime",
			"ApplicationCore",
			"EMMartialArts",
			"EMMartialArtsEditor",
			"Persona"
		});
}
}
