// Fill out your copyright notice in the Description page of Project Settings.


#include "FadeWidget.h"

#include "Animation/WidgetAnimation.h"

void UFadeWidget::PlayFadeOut()
{
	if (FadeAnim)
	{
		PlayAnimation(FadeAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
	}
	
}

void UFadeWidget::PlayFadeIn()
{
	if (FadeAnim)
	{
		PlayAnimation(FadeAnim, 0.0f, 1, EUMGSequencePlayMode::Reverse, 1.0f);
	}
}

void UFadeWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (FadeAnim)
	{
		FWidgetAnimationDynamicEvent AnimEvent;
		AnimEvent.BindDynamic(this, &UFadeWidget::OnFadeOutCompletedCallback);
		BindToAnimationFinished(FadeAnim, AnimEvent);
	}
}

void UFadeWidget::OnFadeOutCompletedCallback()
{
	if (OnFadeOutCompleted.IsBound())
	{
		OnFadeOutCompleted.Broadcast();
	}
}