// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "TickRiskTypes.h"

class FLevelPerformanceInspectorEditorModule;


class SLevelPerformanceInspectorTab: public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLevelPerformanceInspectorTab) {}
		SLATE_ARGUMENT(
		   FLevelPerformanceInspectorEditorModule*,
			   EditorModule
		)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
private:
	FLevelPerformanceInspectorEditorModule* EditorModule;

	FReply OnScanClicked();

	TArray<TSharedPtr<FTickRiskResult>> Results;
	TSharedPtr<SListView<TSharedPtr<FTickRiskResult>>> ListView;

	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FTickRiskResult> Item,
		const TSharedRef<STableViewBase>& OwnerTable);

	void JumpToObject(TSharedPtr<FTickRiskResult> Item);

	FTickScanSummary ScanSummary;
	TSharedPtr<STextBlock> SummaryText;

};
