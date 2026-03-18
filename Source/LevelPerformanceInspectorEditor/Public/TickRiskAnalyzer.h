// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TickRiskTypes.h"



class  FTickRiskAnalyzer
{
public:
	static ETickRiskLevel AnalyActor(const AActor* Actor, FString& OutReason);
	static ETickRiskLevel AnalyComponent(const class UActorComponent* Component, FString& OutReason);
};
