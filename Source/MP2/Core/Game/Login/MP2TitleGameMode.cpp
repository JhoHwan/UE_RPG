// Fill out your copyright notice in the Description page of Project Settings.


#include "MP2TitleGameMode.h"
#include "Core/SubSystem/MP2HttpSubSystem.h"

void AMP2TitleGameMode::TryLogin(const FString& Email, const FString& Password, FOnLoginResponse Callback)
{
	OnLoginResponseCallback = Callback;

	SendAuthRequest(LoginUrl, Email, Password,
		FOnHttpRequestComplete::CreateUObject(this, &AMP2TitleGameMode::OnLoginResponse));
}

void AMP2TitleGameMode::OnLoginResponse(const FAPIResponse& Response)
{
	UE_LOG(LogTemp, Log, TEXT("%s"), *Response.Message);

	if (!Response.Error)
	{
		if (Response.Data.IsValid() && Response.Data->HasField("token"))
		{ 
			AuthInformation.bLogin = true;
			AuthInformation.Token = Response.Data->GetStringField("token");
		}
	}

	OnLoginResponseCallback.Execute(!Response.Error, Response.Message);
	OnLoginResponseCallback.Unbind();
}

void AMP2TitleGameMode::TryRegister(const FString& Email, const FString& Password, FOnRegisterResponse Callback)
{
	OnRegisterResponseCallback = Callback;

	SendAuthRequest(RegisterUrl, Email, Password,
		FOnHttpRequestComplete::CreateUObject(this, &AMP2TitleGameMode::OnRegisterResponse));
}

void AMP2TitleGameMode::OnRegisterResponse(const FAPIResponse& Response)
{
	UE_LOG(LogTemp, Log, TEXT("%s"), *Response.Message);

	OnRegisterResponseCallback.Execute(!Response.Error, Response.Message);
	OnRegisterResponseCallback.Unbind();
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
