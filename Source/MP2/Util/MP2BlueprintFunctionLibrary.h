// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MP2BlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class MP2_API UMP2BlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void SetFocusToWidget(class UWorld* World, class UWidget* Widget);

	UFUNCTION(BlueprintCallable)
	static bool IsValidEmailFormat(const FString& Email);
};
