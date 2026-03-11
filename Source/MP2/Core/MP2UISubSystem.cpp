// Fill out your copyright notice in the Description page of Project Settings.


#include "MP2UISubSystem.h"

#include "MP2GameInstance.h"
#include "UI/MP2FadeWidget.h"

void UMP2UISubSystem::StartFadeOut()
{
	if (UMP2FadeWidget* FadeWidget = GetOrCreateFadeWidget())
	{
		FadeWidget->PlayFadeOut();
	}
}

void UMP2UISubSystem::StartFadeIn()
{
	if (UMP2FadeWidget* FadeWidget = GetOrCreateFadeWidget())
	{
		FadeWidget->PlayFadeIn();
	}
}

void UMP2UISubSystem::AddFadeToViewPort()
{
	if (FadeWidgetInstance && !FadeWidgetInstance->IsInViewport())
	{
		FadeWidgetInstance->AddToViewport(9999);
	}
}

void UMP2UISubSystem::HandleFadeOutCompleted()
{
	if(OnFadeOutCompletedDispatcher.IsBound())
	{
		OnFadeOutCompletedDispatcher.Broadcast();
		OnFadeOutCompletedDispatcher.Clear();
	}
}

UMP2FadeWidget* UMP2UISubSystem::GetOrCreateFadeWidget()
{
	if (!FadeWidgetInstance)
	{
		TSubclassOf<UMP2FadeWidget> FadeWidgetClass = Cast<UMP2GameInstance>(GetGameInstance())->FadeWidgetClass;
		if (FadeWidgetClass)
		{
			FadeWidgetInstance = CreateWidget<UMP2FadeWidget>(GetWorld(), FadeWidgetClass);
        
			if (FadeWidgetInstance)
			{
				// 위젯이 처음 생성될 때만 딱 한 번 내부 이벤트를 연결
				FadeWidgetInstance->OnFadeOutCompleted.AddUObject(this, &UMP2UISubSystem::HandleFadeOutCompleted);
			}
		}
	}

	AddFadeToViewPort();

	return FadeWidgetInstance;
}
