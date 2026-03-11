// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MP2FadeWidget.generated.h"

/**
 * 
 */
UCLASS()
class MP2_API UMP2FadeWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void PlayFadeOut();
	void PlayFadeIn();

protected:
	virtual void NativeOnInitialized() override;
	
private:
	UFUNCTION()
	void OnFadeOutCompletedCallback();
	
public:
	FSimpleMulticastDelegate OnFadeOutCompleted;
	
protected:
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeAnim;

};
