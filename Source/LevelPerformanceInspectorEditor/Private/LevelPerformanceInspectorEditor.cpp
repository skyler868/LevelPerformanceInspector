// Copyright Epic Games, Inc. All Rights Reserved.

#include "LevelPerformanceInspectorEditor.h"
#include "ToolMenus.h"
#include "Editor.h"
#include "EngineUtils.h" // 里有TActorIterator
#include "TickRiskAnalyzer.h"
#include "SLevelPerformanceInspectorTab.h"
#include "CoreMinimal.h"

#define LOCTEXT_NAMESPACE "FLevelPerformanceInspectorEditorModule"  

void FLevelPerformanceInspectorEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	

	if (UToolMenus::IsToolMenuUIEnabled()) // ToolMenus 系统 是否已经初始化
	{
		RegisterMenus(); // 注册菜单 热加载 / Live Coding
	}
	else
	{
		UToolMenus::RegisterStartupCallback(
			FSimpleMulticastDelegate::FDelegate::CreateRaw(
				this,
				&FLevelPerformanceInspectorEditorModule::RegisterMenus
			) 
		); // 未初始化（冷启动） 注册一个等菜单系统准备好再执行的回调
	}

	// TabSpawner：只在模块启动时注册
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		"LevelPerformanceInspectorTab",
		FOnSpawnTab::CreateRaw(
			this,
			&FLevelPerformanceInspectorEditorModule::SpawnInspectorTab
		)
	)
		.SetDisplayName(FText::FromString("Performance Inspector"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

}

void FLevelPerformanceInspectorEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UToolMenus::UnregisterOwner(this);
}

void FLevelPerformanceInspectorEditorModule::RunScan(TArray<TSharedPtr<struct FTickRiskResult>>& OutResults,
	FTickScanSummary& OutSummary)
{
	OutResults.Reset();
	UWorld* World = GEditor->GetEditorWorldContext().World(); // 在正在编辑的关卡中得到UWorld

	if (!World) {
	
		return;
	}

	for (TActorIterator<AActor> It(World); It; ++It) {
		AActor* Actor = *It;  // 扫描关卡结构
		FString Reason;
		
		ETickRiskLevel Risk = FTickRiskAnalyzer::AnalyActor(Actor, Reason);
		
		OutResults.Add(MakeShared<FTickRiskResult>(Actor->GetName(),
			Actor->GetName(),
					Risk,
			Reason,
			Actor,false));
		
		if (Risk == ETickRiskLevel::High) {
			OutSummary.HighRiskCount++;
		}

		OutSummary.ActorCount++;
	

		TArray<UActorComponent*> Components;
		Actor->GetComponents(Components);
		for (UActorComponent* Comp : Components) {
			Risk = FTickRiskAnalyzer::AnalyComponent(Comp, Reason);
			
			OutResults.Add(MakeShared<FTickRiskResult>(
				Comp->GetName(),
				Actor->GetName(),
				Risk,
				Reason,
				Actor,
				true
				));
			if (Risk == ETickRiskLevel::High) {
				OutSummary.HighRiskCount++;
				
			}
			OutSummary.ComponentCount++;
		}

	}

	UE_LOG(LogTemp, Warning,
		TEXT("Tick Scan Finished | Actors: %d | Components: %d | HighRisk: %d"),
		OutSummary.ActorCount,
		OutSummary.ComponentCount,
		OutSummary.HighRiskCount
	);

}

TSharedRef<SDockTab> FLevelPerformanceInspectorEditorModule::SpawnInspectorTab(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab).TabRole(ETabRole::NomadTab)
		[
			SNew(SLevelPerformanceInspectorTab)
				.EditorModule(this)
		];
}


void FLevelPerformanceInspectorEditorModule::RegisterMenus() {


	UToolMenu* Menu =
		UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");  // Get Tools 主菜单 的可扩展接口

	FToolMenuSection& Section =
		Menu->AddSection("LevelPerformanceInspector", FText::FromString("Performance")); // 菜单分组 
	//                              ID               Editor UI
	Section.AddMenuEntry(  // 加入可点击功能
		"PerfScanDummy",  
		FText::FromString("Performance Scan"),
		FText::FromString("Test plugin menu"),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateRaw(
				this, 
				&FLevelPerformanceInspectorEditorModule::OnMenuClicked
			)
		)
		);

}

void FLevelPerformanceInspectorEditorModule::OnMenuClicked() {
	
	FGlobalTabmanager::Get()->TryInvokeTab(
		FTabId("LevelPerformanceInspectorTab")
	);

	
}



#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLevelPerformanceInspectorEditorModule, LevelPerformanceInspectorEditor)