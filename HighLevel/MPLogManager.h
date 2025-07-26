#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "CommonEnum.h"
#include "CommonStruct.h"
#include "MPLogManager.generated.h"

UCLASS(BlueprintType, Blueprintable)
class MEOWPHONE_API UMPLogManager : public UObject
{
	GENERATED_BODY()

public:
	UMPLogManager();

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