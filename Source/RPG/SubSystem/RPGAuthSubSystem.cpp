// Fill out your copyright notice in the Description page of Project Settings.


#include "SubSystem/RPGAuthSubSystem.h"
#include "RPGHttpSubSystem.h"

void URPGAuthSubSystem::TryLogin(const FString& Email, const FString& Password, FOnLoginResponse Callback)
{
	OnLoginResponseCallback = Callback;

	SendAuthRequest(LoginUrl, Email, Password,
		FOnHttpRequestComplete::CreateUObject(this, &URPGAuthSubSystem::OnLoginResponse));
}

void URPGAuthSubSystem::OnLoginResponse(const FAPIResponse& Response)
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

void URPGAuthSubSystem::TryRegister(const FString& Email, const FString& Password, FOnRegisterResponse Callback)
{
	OnRegisterResponseCallback = Callback;

	SendAuthRequest(RegisterUrl, Email, Password,
		FOnHttpRequestComplete::CreateUObject(this, &URPGAuthSubSystem::OnRegisterResponse));
}

void URPGAuthSubSystem::OnRegisterResponse(const FAPIResponse& Response)
{
	UE_LOG(LogTemp, Log, TEXT("%s"), *Response.Message);

	OnRegisterResponseCallback.Execute(!Response.Error, Response.Message);
	OnRegisterResponseCallback.Unbind();
}

void URPGAuthSubSystem::SendAuthRequest(const FString& Url, const FString& Email, const FString& Password, FOnHttpRequestComplete Callback)
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
