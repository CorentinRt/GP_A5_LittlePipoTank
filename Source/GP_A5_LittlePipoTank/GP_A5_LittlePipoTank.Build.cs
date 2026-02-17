// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GP_A5_LittlePipoTank : ModuleRules
{
	public GP_A5_LittlePipoTank(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDefinitions.Add("NOMINMAX");
		PublicDefinitions.Add("WIN32_LEAN_AND_MEAN");
		
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"ENet6Library"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"GP_A5_LittlePipoTank",
			"GP_A5_LittlePipoTank/Variant_Platforming",
			"GP_A5_LittlePipoTank/Variant_Platforming/Animation",
			"GP_A5_LittlePipoTank/Variant_Combat",
			"GP_A5_LittlePipoTank/Variant_Combat/AI",
			"GP_A5_LittlePipoTank/Variant_Combat/Animation",
			"GP_A5_LittlePipoTank/Variant_Combat/Gameplay",
			"GP_A5_LittlePipoTank/Variant_Combat/Interfaces",
			"GP_A5_LittlePipoTank/Variant_Combat/UI",
			"GP_A5_LittlePipoTank/Variant_SideScrolling",
			"GP_A5_LittlePipoTank/Variant_SideScrolling/AI",
			"GP_A5_LittlePipoTank/Variant_SideScrolling/Gameplay",
			"GP_A5_LittlePipoTank/Variant_SideScrolling/Interfaces",
			"GP_A5_LittlePipoTank/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
