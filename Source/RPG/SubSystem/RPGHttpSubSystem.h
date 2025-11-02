// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RPGHttpTypes.h"
#include "RPGHttpSubSystem.generated.h"

/**
 * 
 */

UCLASS()
class RPG_API URPGHttpSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	void SendRequest(const FString& Url, const FString& Inverb, const FString& Content, FOnHttpRequestComplete Callback, bool bUseAuth = false);

private:
	const FString DefaultUrl{ TEXT("https://localhost:7013") };
};
