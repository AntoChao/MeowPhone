#pragma once

#include "CoreMinimal.h"
#include "CommonStruct.generated.h"

class AMissionZone;
class MPCharacter;

enum class EMissionTaskType : uint8;
enum class EMissionTaskStatus : uint8;

USTRUCT(BlueprintType)
struct FSessionInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
	int32 sessionIndex;

	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
	FString sessionName;

	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
	int32 curPlayersNum;

	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
	int32 maxPlayersNum;
};


USTRUCT(BlueprintType)
struct FMissionDetails
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
    int missionID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
    FText missionName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
    UTexture2D* MissionIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
    int32 Difficulty;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
    int32 Reward; // experience

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
    TArray<FMissionTasks> allMissionTasks;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
    bool alreadyCompleted = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
    AMissionZone* missionZone = nullptr;

};


USTRUCT(BlueprintType)
struct FMissionTasks
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
    EMissionTaskType missionTaskType = EMissionTaskType::EStealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
    FText missionTaskName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
    FText missionTaskDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
    EMissionTaskStatus missionTaskStatus = EMissionTaskStatus::ENoActive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
    bool isTimeConstraint = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
    int timeLimit = 0;
};


USTRUCT(BlueprintType)
struct FMissionZoneAIInfo
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    int AINums = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    TArray<FName> AICharactersNames;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    TArray<FVector> SpawnLocations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    TArray<FVector> PatrolPoints; // redesign probably
};