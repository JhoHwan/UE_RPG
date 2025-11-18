// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/Types/MP2HttpTypes.h"
#include "MP2HttpSubSystem.generated.h"

/**
 * 
 */

UCLASS()
class MP2_API UMP2HttpSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	void SendRequest(const FString& Url, const FString& Inverb, const FString& Content, FOnHttpRequestComplete Callback, bool bUseAuth = false, const FAuthInfo* AuthInfo = nullptr);

private:
	const FString DefaultUrl{ TEXT("https://localhost:7013") };
};
