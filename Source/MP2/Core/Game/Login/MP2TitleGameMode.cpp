// Fill out your copyright notice in the Description page of Project Settings.


#include "MP2TitleGameMode.h"
#include "Core/SubSystem/MP2HttpSubSystem.h"
#include "Core/SaveGame/MP2LoginSaveGame.h"
#include "Kismet/GameplayStatics.h"

void AMP2TitleGameMode::TryLogin(const FString& Email, const FString& Password)
{
	SendAuthRequest(LoginUrl, Email, Password,
		FOnHttpRequestComplete::CreateUObject(this, &AMP2TitleGameMode::LoginResultHandler));
}

void AMP2TitleGameMode::LoginResultHandler(const FAPIResponse& Response)
{
	bool bSuccess = Response.ErrorCode == 0;
	FString Message = ErrorCodeToMessage(Response.ErrorCode);

	if (bSuccess)
	{
		if (Response.Data.IsValid() && Response.Data->HasField(TEXT("token")))
		{
			UMP2HttpSubSystem* HttpSubSystem = GetGameInstance()->GetSubsystem<UMP2HttpSubSystem>();
			HttpSubSystem->LogIn(Response.Data->GetStringField(TEXT("token")));
		}
		else
		{
			bSuccess = false;
			Message = ErrorCodeToMessage(5);
		}
	}

	if (OnLoginResponse.IsBound())
	{
		OnLoginResponse.Broadcast(bSuccess, Message);
	}
}

void AMP2TitleGameMode::TryRegister(const FString& Email, const FString& Password)
{
	SendAuthRequest(RegisterUrl, Email, Password,
		FOnHttpRequestComplete::CreateUObject(this, &AMP2TitleGameMode::RegisterResultHandler));
}

void AMP2TitleGameMode::RegisterResultHandler(const FAPIResponse& Response)
{
	bool bSuccess = Response.ErrorCode == 0;

	if (OnRegisterResponse.IsBound())
	{
		OnRegisterResponse.Broadcast(bSuccess, ErrorCodeToMessage(Response.ErrorCode));
	}
}

void AMP2TitleGameMode::SendAuthRequest(const FString& Url, const FString& Email, const FString& Password, FOnHttpRequestComplete Callback)
{
	UMP2HttpSubSystem* HttpSubSystem = GetGameInstance()->GetSubsystem<UMP2HttpSubSystem>();
	if (!HttpSubSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMP2HttpSubSystem not found."));
		return;
	}

	TSharedPtr<FJsonObject> Json = MakeShared<FJsonObject>();
	Json->SetStringField("email", Email);
	Json->SetStringField("password", Password);

	FString Content;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Content);
	FJsonSerializer::Serialize(Json.ToSharedRef(), Writer);

	HttpSubSystem->SendRequest(Url, TEXT("POST"), Content, Callback);
}

FString AMP2TitleGameMode::ErrorCodeToMessage(int32 InErrorCode)
{
	if (InErrorCode == 0) return {};
	FString OutMessage;
	switch (InErrorCode)
	{
	case 2:
		OutMessage = TEXT("서버와 연결할 수 없습니다.\n잠시 후 다시 시도하세요.");
		break;
	case 1001:
		OutMessage = TEXT("로그인 정보가 올바르지 않습니다");
		break;
	case 1002:
		OutMessage = TEXT("정지된 계정입니다");
		break;
	case 1010:
		OutMessage = TEXT("이미 가입된 이메일입니다");
		break;
	default:
		OutMessage = FString::Printf(TEXT("알 수 없는 오류가 발생했습니다.\n(Code: %d)"), InErrorCode);
		break;
	}
	return OutMessage;
}