// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MP2UISubSystem.generated.h"

class UMP2FadeWidget;
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFadeCompleted);

UCLASS()
class MP2_API UMP2UISubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	void StartFadeOut();
	void StartFadeIn();
	void AddFadeToViewPort();
	
private:
	void HandleFadeOutCompleted();
	UMP2FadeWidget* GetOrCreateFadeWidget();
	
public:
	UPROPERTY(BlueprintAssignable, Category = "UI|Fade")
	FOnFadeCompleted OnFadeOutCompletedDispatcher;
	
protected:
	UPROPERTY()
	TObjectPtr<UMP2FadeWidget> FadeWidgetInstance;
};
