// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MeowPhone : ModuleRules
{
	public MeowPhone(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", 
			"EnhancedInput", "UMG", "MediaAssets", "AIModule", "GeometryCollectionEngine", "Chaos",
            "MotionWarping", "AnimGraphRuntime"});

        PublicDependencyModuleNames.AddRange(new string[] { "NavigationSystem"});

        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "OnlineSubsystemNull", "OnlineSubsystemSteam" });

    }
}
