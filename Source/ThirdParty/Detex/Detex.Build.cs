using UnrealBuildTool;
using System.IO;

public class Detex : ModuleRules {
	public Detex(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.NoPCHs;

		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "ThirdParty/detex"));
		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "ThirdParty/detex"));

		/* The vendored C code predates the secure CRT, and this is what its warnings ask for */
		PrivateDefinitions.Add("_CRT_SECURE_NO_WARNINGS");

		PrivateDependencyModuleNames.AddRange(new[] {
			"Core",
			"CoreUObject"
		});
	}
}