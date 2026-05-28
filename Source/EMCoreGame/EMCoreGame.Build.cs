using UnrealBuildTool;

public class EMCoreGame : ModuleRules
{
	public EMCoreGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"Paper2D",
			"UMG",
			"SlateCore",
			"Slate",
			"AIModule",
			"GameplayTasks",
			"EMTableCore",
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });
		PublicIncludePaths.Add("EMCoreGame");
	}
}
