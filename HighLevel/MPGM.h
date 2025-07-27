#pragma once

// [Meow-Phone Project]
//
// This is the base Game Mode class for the project. Its primary purpose is to serve as
// a common ancestor for more specialized Game Modes (`AMPGMInit`, `AMPGMGameplay`) and
// to provide a convenient, cached reference to the Game Instance (`UMPGI`).
//
// How to utilize in Blueprint:
// - You should not set this as the Game Mode for your levels. Instead, use one of its child classes.
// - Child Blueprints will inherit the `mpGameInstance` variable, giving them easy access to the Game Instance without needing to cast from a "Get Game Instance" node every time.
//
// Necessary things to define:
// - Nothing needs to be defined in the editor for this base class.
//
// How it interacts with other classes:
// - AGameModeBase: The standard Unreal Engine class it inherits from.
// - UMPGI: It gets and holds a pointer to the Game Instance for easy access by itself and its child classes. The `BeginPlay` function is where this reference is typically assigned.
// - AMPGMInit / AMPGMGameplay: These more specific Game Modes inherit from this class.

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MPGM.generated.h"

class UMPGI;

UCLASS(minimalapi)
class AMPGM : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMPGM();

// common gamemode instance
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
		UMPGI* mpGameInstance;

public:
	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		virtual void BeginPlay() override;
};
