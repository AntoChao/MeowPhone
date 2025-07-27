// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

// [Meow-Phone Project]
//
// This is the module rules file for the MeowPhone game module. It is a C# script
// that the Unreal Build Tool (UBT) uses to determine how to compile the game code.
//
// The most important part of this file is the list of Dependency Modules. This tells UBT
// which other parts of the Unreal Engine your game code relies on. Adding a module here
// allows you to `#include` its header files in your C++ code.
//
// PublicDependencyModuleNames:
// - Core, CoreUObject, Engine, InputCore: The absolute basic engine modules.
// - EnhancedInput: For the modern input system (Input Actions, Input Mapping Contexts).
// - UMG: The Unreal Motion Graphics module, for all UI/widget functionality.
// - AIModule, NavigationSystem: For all AI behavior (Behavior Trees, Blackboards, Perception) and pathfinding.
// - GeometryCollectionEngine, Chaos: For the Chaos physics-based destruction system (breakable objects).
// - MotionWarping: For the animation system that dynamically adjusts root motion.
// - OnlineSubsystem, OnlineSubsystemSteam: For all multiplayer session management, connecting to the Steam backend.
//
// PrivateDependencyModuleNames:
// - Slate, SlateCore: Lower-level UI frameworks that UMG is built upon. Needed for some advanced UI customization.

public class MeowPhone : ModuleRules
{
	public MeowPhone(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "InputCore",
            "EnhancedInput", "UMG", "MediaAssets",
            "AIModule", "GeometryCollectionEngine", "Chaos",
            "MotionWarping", "AnimGraphRuntime", "NavigationSystem",
            "OnlineSubsystem", "OnlineSubsystemUtils", "OnlineSubsystemSteam"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Slate",
            "SlateCore"
        });
    }
}
