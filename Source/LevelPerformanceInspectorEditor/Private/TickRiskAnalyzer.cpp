// Fill out your copyright notice in the Description page of Project Settings.


#include "TickRiskAnalyzer.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"

ETickRiskLevel FTickRiskAnalyzer::AnalyActor(const AActor* Actor, FString& OutReason)
{
	if (!Actor)
	{
		OutReason = TEXT("Actor is null");
		return ETickRiskLevel::Safe;
	}

	const FTickFunction& Tick = Actor->PrimaryActorTick;

	// 不支持 Tick
	if (!Tick.bCanEverTick)
	{
		OutReason = TEXT("Actor does not support ticking");
		return ETickRiskLevel::Safe;
	}

	// 默认启用 Tick
	if (Tick.bStartWithTickEnabled)
	{
		// 每帧 Tick
		if (Tick.TickInterval <= 0.f)
		{
			OutReason = TEXT("Actor ticks every frame by default");
			return ETickRiskLevel::High;
		}
		OutReason = TEXT("Actor ticks with interval");
		return ETickRiskLevel::Medium;
	}

	// 支持 Tick 但默认不开
	OutReason = TEXT("Actor supports tick but starts disabled");
	return ETickRiskLevel::Low;
}

ETickRiskLevel FTickRiskAnalyzer::AnalyComponent(const UActorComponent* Component, FString& OutReason)
{

	if (!Component)
	{
		
		return ETickRiskLevel::Safe;
	}

	const FTickFunction& Tick = Component->PrimaryComponentTick;

	// 根本不支持 Tick
	if (!Tick.bCanEverTick)
	{
		OutReason = TEXT("Component does not support ticking");
		return ETickRiskLevel::Safe;
	}

	// 默认会启用 Tick（重点）
	if (Tick.bStartWithTickEnabled)
	{
		// 每帧 Tick
		if (Tick.TickInterval <= 0.f)
		{
			OutReason = TEXT("Component ticks every frame by default");
			return ETickRiskLevel::High;
		}

		// 有间隔 Tick
		OutReason = TEXT("Component ticks with interval");
		return ETickRiskLevel::Medium;
	}

	// 设计上支持 Tick，但默认不开
	OutReason = TEXT("Component supports tick but starts disabled");
	return ETickRiskLevel::Low;

}
