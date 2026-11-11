using UnrealBuildTool;

public class CumaPlay : ModuleRules
{
	public CumaPlay(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"SunPosition"
		});
	}
}
