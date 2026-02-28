// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Core/Types/MP2HttpTypes.h"
#include "MP2TitleGameMode.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoginResponse, bool, bSuccess, const FString&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRegisterResponse, bool, bSuccess, const FString&, Message);

UCLASS()
class MP2_API AMP2TitleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void TryLogin(const FString& Email, const FString& Password);


	UFUNCTION(BlueprintCallable)
	void TryRegister(const FString& Email, const FString& Password);

private:

	void LoginResultHandler(const FAPIResponse& Response);
	void RegisterResultHandler(const FAPIResponse& Response);

	void SendAuthRequest(const FString& Url, const FString& Email, const FString& Password, FOnHttpRequestComplete Callback);
	FString ErrorCodeToMessage(int32 InErrorCode);

private:
	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = "true"))
	FOnLoginResponse OnLoginResponse;

	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = "true"))
	FOnRegisterResponse OnRegisterResponse;

private:
	const FString LoginUrl{TEXT("/api/Auth/Login")};
	const FString RegisterUrl{ TEXT("/api/Auth/Register") };
};
