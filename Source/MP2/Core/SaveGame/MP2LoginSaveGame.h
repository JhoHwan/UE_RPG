// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MP2LoginSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class MP2_API UMP2LoginSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UMP2LoginSaveGame();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FString GetSlotName() { return SaveSlotName; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE FText GetEmailText() const { return FText::FromString(Email); }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetEmail(FText InEmail){ Email = InEmail.ToString(); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE bool IsRemember() const { return bRemember; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetRemember(bool InRemember) { bRemember = InRemember; }


public:
	static const FString SaveSlotName;

	UPROPERTY()
	FString Email;

	UPROPERTY()
	bool bRemember;

};
