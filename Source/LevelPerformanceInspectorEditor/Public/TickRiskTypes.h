#pragma once


#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"

enum class ETickRiskLevel : uint8
{
	Safe,
	Low,
	Medium,
	High
};

struct FTickRiskResult
{
	FString ObjectName;
	FString OwnerName;
	ETickRiskLevel RiskLevel;
	FString Reason;
	TWeakObjectPtr<UObject> SourceObject;
	bool bIsComponent = false;

	FTickRiskResult(
		const FString& InObjectName,
		const FString& InOwner,
		ETickRiskLevel InRisk,
		FString Reason,
		TWeakObjectPtr<UObject> InSource,
		bool bIsComponent

	)
		: ObjectName(InObjectName)
		, OwnerName(InOwner)
		, RiskLevel(InRisk)
		, Reason(Reason)
		, SourceObject(InSource),
		bIsComponent(bIsComponent)
	{
	}
};

struct FTickScanSummary
{
	int32 ActorCount = 0;
	int32 ComponentCount = 0;
	int32 HighRiskCount = 0;

	void Reset()
	{
		ActorCount = 0;
		ComponentCount = 0;
		HighRiskCount = 0;
	}
};

//using FTickRiskResultPtr = TSharedPtr<FTickRiskResult>;

