#pragma once

// [Meow-Phone Project]
//
// This is a global, static logging manager. It provides a centralized and configurable way
// to output log messages to the screen and the console. Because all its functions are static,
// you do not need to create an instance of this class.
//
// How to utilize in Blueprint:
// 1. From any Blueprint, you can directly call the static functions on `UManagerLog`.
// 2. To log an error, call `UManagerLog::LogError`. Pass in the message and an optional context string.
// 3. Similarly, use `LogWarning`, `LogInfo`, etc., for different levels of logging.
// 4. In a central place, like the Game Instance on startup, you can call `SetLogConfig` to configure the logging behavior, such as the minimum log level to display or whether to log to the screen.
//
// Necessary things to define:
// - There are no properties to set in the editor. All configuration is done at runtime via the `SetLogConfig` function, which takes an `FLogConfig` struct.
// - The `FLogConfig` struct, likely defined in `CommonStruct.h`, needs to be created in Blueprints or C++ and passed to `SetLogConfig`.
//
// How it interacts with other classes:
// - UManagerMP: Inherits from the base manager class, though this is less relevant since its functions are static and it is not instantiated.
// - FLogConfig (Struct): This struct is used to configure the logger's behavior (e.g., setting the log level).
// - ELogLevel (Enum): This enum defines the different severity levels for log messages (Error, Warning, Info, etc.).
// - Any Blueprint/Class: Any part of the game's code or Blueprint graph can call the static Log functions to output messages, making it a globally accessible utility.
// - UEngine: It uses the global `GEngine` object to print messages to the screen via `AddOnScreenDebugMessage`.

#include "CoreMinimal.h"
#include "Engine/Engine.h"

#include "ManagerMP.h"
#include "ManagerLog.generated.h"

enum class ELogLevel : uint8;
struct FLogConfig;

UCLASS(BlueprintType, Blueprintable)
class UManagerLog : public UManagerMP
{
	GENERATED_BODY()

public:
	UManagerLog();

	// Initialize the logging system
	UFUNCTION(BlueprintCallable, Category = "Logging")
	static void InitializeLogging();

	// Logging methods with different levels
	UFUNCTION(BlueprintCallable, Category = "Logging")
	static void LogError(const FString& message, const FString& context = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "Logging")
	static void LogWarning(const FString& message, const FString& context = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "Logging")
	static void LogInfo(const FString& message, const FString& context = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "Logging")
	static void LogDebug(const FString& message, const FString& context = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "Logging")
	static void LogVerbose(const FString& message, const FString& context = TEXT(""));

	// Set logging configuration
	UFUNCTION(BlueprintCallable, Category = "Logging")
	static void SetLogConfig(const FLogConfig& newConfig);

	// Get current logging configuration
	UFUNCTION(BlueprintCallable, Category = "Logging")
	static FLogConfig GetLogConfig() { return LogConfig; }

	// Check if a log level should be displayed
	UFUNCTION(BlueprintCallable, Category = "Logging")
	static bool ShouldLog(ELogLevel level);

	// Format log message
	UFUNCTION(BlueprintCallable, Category = "Logging")
	static FString FormatLogMessage(ELogLevel level, const FString& message, const FString& context);

private:
	// Current logging configuration
	static FLogConfig LogConfig;

	// Internal logging method
	static void InternalLog(ELogLevel level, const FString& message, const FString& context);

	// Get color for log level
	static FColor GetLogColor(ELogLevel level);

	// Get log level string
	static FString GetLogLevelString(ELogLevel level);
}; 