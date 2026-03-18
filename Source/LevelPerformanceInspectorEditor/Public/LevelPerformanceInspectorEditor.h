// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once


#include "Modules/ModuleManager.h"

struct FTickScanSummary;
class FLevelPerformanceInspectorEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void RunScan(TArray<TSharedPtr<struct FTickRiskResult>>& OutResults,
		FTickScanSummary& OutSummary);

	TSharedRef<SDockTab> SpawnInspectorTab(const FSpawnTabArgs& Args);

private:
	void RegisterMenus();
	void OnMenuClicked();
};
