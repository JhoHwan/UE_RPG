// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "MP2LoginLabel.generated.h"

/**
 * 
 */
UCLASS()
class MP2_API UMP2LoginLabel : public UTextBlock
{
	GENERATED_BODY()
	
public:
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = "ErrorLabel")
	void SetErrorLabel(const FString& Message, bool bSetTimer = true, float Time = 3.0f);

private:
	void SetInvisibleTimer(float Time);

private:
	FTimerHandle InvisibleTimer;
};
