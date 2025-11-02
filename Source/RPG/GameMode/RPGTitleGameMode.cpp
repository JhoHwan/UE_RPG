// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGTitleGameMode.h"
#include "SubSystem/RPGHttpSubSystem.h"

void ARPGTitleGameMode::TryLogin(const FString& Email, const FString& Password, FOnLoginResponse Callback)
{
	OnLoginResponseCallback = Callback;

	SendAuthRequest(LoginUrl, Email, Password,
		FOnHttpRequestComplete::CreateUObject(this, &ARPGTitleGameMode::OnLoginResponse));
}

void ARPGTitleGameMode::OnLoginResponse(const FAPIResponse& Response)
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

void ARPGTitleGameMode::TryRegister(const FString& Email, const FString& Password, FOnRegisterResponse Callback)
{
	OnRegisterResponseCallback = Callback;

	SendAuthRequest(RegisterUrl, Email, Password,
		FOnHttpRequestComplete::CreateUObject(this, &ARPGTitleGameMode::OnRegisterResponse));
}

void ARPGTitleGameMode::OnRegisterResponse(const FAPIResponse& Response)
{
	UE_LOG(LogTemp, Log, TEXT("%s"), *Response.Message);

	OnRegisterResponseCallback.Execute(!Response.Error, Response.Message);
	OnRegisterResponseCallback.Unbind();
}

void ARPGTitleGameMode::SendAuthRequest(const FString& Url, const FString& Email, const FString& Password, FOnHttpRequestComplete Callback)
{
	URPGHttpSubSystem* HttpSubSystem = GetGameInstance()->GetSubsystem<URPGHttpSubSystem>();
	if (!HttpSubSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("URPGHttpSubSystem not found."));
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
