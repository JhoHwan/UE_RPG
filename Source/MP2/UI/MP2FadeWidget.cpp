// Fill out your copyright notice in the Description page of Project Settings.


#include "MP2FadeWidget.h"

#include "Animation/WidgetAnimation.h"

void UMP2FadeWidget::PlayFadeOut()
{
	if (FadeAnim)
	{
		PlayAnimation(FadeAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
	}
	
}

void UMP2FadeWidget::PlayFadeIn()
{
	if (FadeAnim)
	{
		PlayAnimation(FadeAnim, 0.0f, 1, EUMGSequencePlayMode::Reverse, 1.0f);
	}
}

void UMP2FadeWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (FadeAnim)
	{
		FWidgetAnimationDynamicEvent AnimEvent;
		AnimEvent.BindDynamic(this, &UMP2FadeWidget::OnFadeOutCompletedCallback);
		BindToAnimationFinished(FadeAnim, AnimEvent);
	}
}

void UMP2FadeWidget::OnFadeOutCompletedCallback()
{
	if (OnFadeOutCompleted.IsBound())
	{
		OnFadeOutCompleted.Broadcast();
	}
}