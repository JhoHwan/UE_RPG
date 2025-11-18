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

public:
	static FString SaveSlotName;

	UPROPERTY(VisibleAnywhere)
	FString Email;

	UPROPERTY(VisibleAnywhere)
	bool bRememberEmail;

};
