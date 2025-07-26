#include "MPLogManager.h"
#include "Engine/Engine.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/DateTime.h"

// Initialize static member
FLogConfig UMPLogManager::LogConfig;

UMPLogManager::UMPLogManager()
{
}

void UMPLogManager::InitializeLogging()
{
	// Set default configuration
	LogConfig.logLevel = ELogLevel::Warning;
	LogConfig.enableScreenLogging = false;
	LogConfig.enableFileLogging = false;
	LogConfig.screenLogDuration = 5.0f;

	LogInfo(TEXT("Logging system initialized"), TEXT("MPLogManager"));
}

void UMPLogManager::LogError(const FString& message, const FString& context)
{
	InternalLog(ELogLevel::Error, message, context);
}

void UMPLogManager::LogWarning(const FString& message, const FString& context)
{
	InternalLog(ELogLevel::Warning, message, context);
}

void UMPLogManager::LogInfo(const FString& message, const FString& context)
{
	InternalLog(ELogLevel::Info, message, context);
}

void UMPLogManager::LogDebug(const FString& message, const FString& context)
{
	InternalLog(ELogLevel::Debug, message, context);
}

void UMPLogManager::LogVerbose(const FString& message, const FString& context)
{
	InternalLog(ELogLevel::Verbose, message, context);
}

void UMPLogManager::SetLogConfig(const FLogConfig& newConfig)
{
	LogConfig = newConfig;
	LogInfo(TEXT("Logging configuration updated"), TEXT("MPLogManager"));
}

bool UMPLogManager::ShouldLog(ELogLevel level)
{
	return static_cast<uint8>(level) <= static_cast<uint8>(LogConfig.logLevel);
}

FString UMPLogManager::FormatLogMessage(ELogLevel level, const FString& message, const FString& context)
{
	FString formattedMessage = FString::Printf(TEXT("[%s] %s"), *GetLogLevelString(level), *message);
	
	if (!context.IsEmpty())
	{
		formattedMessage = FString::Printf(TEXT("%s [%s]"), *formattedMessage, *context);
	}

	return formattedMessage;
}

void UMPLogManager::InternalLog(ELogLevel level, const FString& message, const FString& context)
{
	if (!ShouldLog(level))
	{
		return;
	}

	FString formattedMessage = FormatLogMessage(level, message, context);
	FColor logColor = GetLogColor(level);

	// Console logging
	switch (level)
	{
	case ELogLevel::Error:
		UE_LOG(LogTemp, Error, TEXT("%s"), *formattedMessage);
		break;
	case ELogLevel::Warning:
		UE_LOG(LogTemp, Warning, TEXT("%s"), *formattedMessage);
		break;
	case ELogLevel::Info:
		UE_LOG(LogTemp, Log, TEXT("%s"), *formattedMessage);
		break;
	case ELogLevel::Debug:
		UE_LOG(LogTemp, Verbose, TEXT("%s"), *formattedMessage);
		break;
	case ELogLevel::Verbose:
		UE_LOG(LogTemp, VeryVerbose, TEXT("%s"), *formattedMessage);
		break;
	default:
		break;
	}

	// Screen logging (if enabled)
	if (LogConfig.enableScreenLogging && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, // Key (use -1 for unique messages)
			LogConfig.screenLogDuration,
			logColor,
			formattedMessage
		);
	}
}

FColor UMPLogManager::GetLogColor(ELogLevel level)
{
	switch (level)
	{
	case ELogLevel::Error:
		return FColor::Red;
	case ELogLevel::Warning:
		return FColor::Yellow;
	case ELogLevel::Info:
		return FColor::White;
	case ELogLevel::Debug:
		return FColor::Blue;
	case ELogLevel::Verbose:
		return FColor::Gray;
	default:
		return FColor::White;
	}
}

FString UMPLogManager::GetLogLevelString(ELogLevel level)
{
	switch (level)
	{
	case ELogLevel::Error:
		return TEXT("ERROR");
	case ELogLevel::Warning:
		return TEXT("WARN");
	case ELogLevel::Info:
		return TEXT("INFO");
	case ELogLevel::Debug:
		return TEXT("DEBUG");
	case ELogLevel::Verbose:
		return TEXT("VERBOSE");
	default:
		return TEXT("UNKNOWN");
	}
} 