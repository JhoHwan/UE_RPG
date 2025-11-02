// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RPGLoginSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API URPGLoginSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	URPGLoginSaveGame();

public:
	static FString SaveSlotName;

	UPROPERTY(VisibleAnywhere)
	FString Email;

	UPROPERTY(VisibleAnywhere)
	bool bRememberEmail;

};
