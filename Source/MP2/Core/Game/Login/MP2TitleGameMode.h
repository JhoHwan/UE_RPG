// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Core/Types/MP2HttpTypes.h"
#include "MP2TitleGameMode.generated.h"

/**
 * 
 */

DECLARE_DELEGATE_TwoParams(FOnLoginResponse, bool /*bSuccess*/, const FString& /*Message*/);
DECLARE_DELEGATE_TwoParams(FOnRegisterResponse, bool /*bSuccess*/, const FString& /*Message*/);

UCLASS()
class MP2_API AMP2TitleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	void TryLogin(const FString& Email, const FString& Password, FOnLoginResponse Callback);
	void OnLoginResponse(const struct FAPIResponse& Response);

	void TryRegister(const FString& Email, const FString& Password, FOnRegisterResponse Callback);
	void OnRegisterResponse(const struct FAPIResponse& Response);


	FORCEINLINE const FAuthInfo& GetAuthInfo() const { return AuthInformation; }

private:
	FOnLoginResponse OnLoginResponseCallback;
	FOnRegisterResponse OnRegisterResponseCallback;

private: 
	void SendAuthRequest(const FString& Url, const FString& Email, const FString& Password, FOnHttpRequestComplete Callback);

private:
	const FString LoginUrl{TEXT("/api/Auth/Login")};
	const FString RegisterUrl{ TEXT("/api/Auth/Register") };


	FAuthInfo AuthInformation{ TEXT(""), false };
};
