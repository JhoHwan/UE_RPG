// Fill out your copyright notice in the Description page of Project Settings.


#include "MP2LoginLabel.h"

void UMP2LoginLabel::BeginDestroy()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearAllTimersForObject(this);
	}

	Super::BeginDestroy();
}

void UMP2LoginLabel::SetErrorLabel(const FString& Message, bool bSetTimer, float Time)
{
	SetVisibility(ESlateVisibility::Visible);
	SetText(FText::FromString(Message));

	if (bSetTimer) SetInvisibleTimer(Time);
}

void UMP2LoginLabel::SetInvisibleTimer(float Time)
{
	if (Time < 0.0f) return;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(InvisibleTimer);
		World->GetTimerManager().SetTimer(InvisibleTimer, [this]() {
			SetVisibility(ESlateVisibility::Collapsed);
			}, Time, false);
	}
}
