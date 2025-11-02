// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "RPGLoginLabel.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API URPGLoginLabel : public UTextBlock
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
