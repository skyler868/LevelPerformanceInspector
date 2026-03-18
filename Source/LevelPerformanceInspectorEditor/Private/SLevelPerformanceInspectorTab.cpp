// Fill out your copyright notice in the Description page of Project Settings.


#include "SLevelPerformanceInspectorTab.h"
#include "LevelPerformanceInspectorEditor.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/SHeaderRow.h"
#include "TickRiskAnalyzer.h"
#include "Editor.h"


FString TickRiskToString(ETickRiskLevel Risk)
{
	switch (Risk)
	{
		case ETickRiskLevel::Safe:    return TEXT("Safe");
		case ETickRiskLevel::Low:    return TEXT("Low");
		case ETickRiskLevel::Medium: return TEXT("Medium");
		case ETickRiskLevel::High:   return TEXT("High");
		default:                     return TEXT("None");
	}
}

FSlateColor GetRiskColor(ETickRiskLevel Risk) {
	float T = static_cast<float>(Risk)
		/ static_cast<float>(ETickRiskLevel::High);

	if (T < 0.5f)
	{
		return FLinearColor::LerpUsingHSV(
			FLinearColor::Green,
			FLinearColor::Yellow,
			T * 2.0f
		);
	}
	else
	{
		return FLinearColor::LerpUsingHSV(
			FLinearColor::Yellow,
			FLinearColor::Red,
			(T - 0.5f) * 2.0f
		);
	}
}


void SLevelPerformanceInspectorTab::Construct(const FArguments& InArgs) {
	EditorModule = InArgs._EditorModule;

	ChildSlot
	[
		SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.FillHeight(1.f)
		.Padding(5)
		[
			
				
			SAssignNew(ListView, SListView<TSharedPtr<FTickRiskResult>>)
			.ItemHeight(28)
			.SelectionMode(ESelectionMode::Single)
			.ListItemsSource(&Results)
			.OnGenerateRow(
				this,
				&SLevelPerformanceInspectorTab::OnGenerateRow
			)
			.HeaderRow
			(
				SNew(SHeaderRow)

				+ SHeaderRow::Column("Object")
				.DefaultLabel(FText::FromString("Object"))
				.HAlignHeader(HAlign_Center)

				+ SHeaderRow::Column("Owner")
				.DefaultLabel(FText::FromString("Owner"))
				.HAlignHeader(HAlign_Center)

				+ SHeaderRow::Column("Risk")
				.DefaultLabel(FText::FromString("Risk"))
				.HAlignHeader(HAlign_Center)

				+ SHeaderRow::Column("Reason")
				.DefaultLabel(FText::FromString("Reason"))
				.HAlignHeader(HAlign_Center)

				+ SHeaderRow::Column("Skip")
				.DefaultLabel(FText::FromString("Skip"))
				.HAlignHeader(HAlign_Center)
			)
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5)
		[
			SNew(SButton)
				.Text(FText::FromString("Scan Tick"))
				.OnClicked(this, &SLevelPerformanceInspectorTab::OnScanClicked)
				.HAlign(HAlign_Center)
		]
			
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5)
			[
				SAssignNew(SummaryText, STextBlock)
					.Text(FText::FromString("No Scan yet"))

			]
	];
	

}

FReply SLevelPerformanceInspectorTab::OnScanClicked()
{
	
	if (!EditorModule) {
		return FReply::Handled();
	}

	Results.Reset();
	EditorModule->RunScan(Results, ScanSummary);
	ListView->RequestListRefresh();

	SummaryText->SetText(
		FText::FromString(
			FString::Printf(
				TEXT("Actors: %d | Components: %d | High Risk: %d"),
				ScanSummary.ActorCount,
				ScanSummary.ComponentCount,
				ScanSummary.HighRiskCount
			)
		)
	);
	
	return FReply::Handled();
}

TSharedRef<ITableRow> SLevelPerformanceInspectorTab::OnGenerateRow(TSharedPtr<FTickRiskResult> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	
	return SNew(STableRow<TSharedPtr<FTickRiskResult>>, OwnerTable)
		[
			SNew(SHorizontalBox)

				+ SHorizontalBox::Slot().FillWidth(0.25f)
				[
					SNew(STextBlock)
						.Text(FText::FromString(Item->ObjectName))
						
				]

				+ SHorizontalBox::Slot().FillWidth(0.25f)
				[
					SNew(STextBlock)
						.Text(FText::FromString(Item->OwnerName))
				]

				+ SHorizontalBox::Slot().FillWidth(0.15f)
				[
					SNew(STextBlock)
						.Text(FText::FromString(
							TickRiskToString(Item->RiskLevel)
						)).ColorAndOpacity(GetRiskColor(Item->RiskLevel))
						
				]

				+ SHorizontalBox::Slot().FillWidth(0.25f)
				[
					SNew(STextBlock)
						.Text(FText::FromString(Item->Reason))
						.AutoWrapText(true)
				]
			+ SHorizontalBox::Slot()
				.FillWidth(0.15f)
				.HAlign(HAlign_Center)
				[
					SNew(SButton)
						.Text(FText::FromString(TEXT("Skip")))
						.OnClicked_Lambda([this, Item]() {
					
						JumpToObject(Item);
						return FReply::Handled();
					})
				]
			    
		];
}

void SLevelPerformanceInspectorTab::JumpToObject(TSharedPtr<FTickRiskResult> Item)
{
	if (!Item.IsValid()) {
		return;
	}

	UObject* obj = Item->SourceObject.Get();
	if (!obj) {
		return;
	}
	GEditor->SelectNone(false, true);

	GEditor->SelectNone(false, true);
	
	

	if (AActor* Actor = Cast<AActor>(obj)) {
		GEditor->SelectActor(Actor, true, true);
		GEditor->MoveViewportCamerasToActor(*Actor, false);
	}
	else if (UActorComponent* Comp = Cast<UActorComponent>(obj)) {
		if (AActor* Owner = Comp->GetOwner())
		{
			GEditor->SelectActor(Owner, true, true);
			GEditor->MoveViewportCamerasToActor(*Owner, false);
		}
	}
}
