// Fill out your copyright notice in the Description page of Project Settings.


#include "MP2HttpSubSystem.h"
#include "Http.h"

void UMP2HttpSubSystem::SendRequest
	(const FString& InUrl, const FString& Inverb, 
	const FString& Content, FOnHttpRequestComplete Callback, bool bUseAuth)
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();

	FString Url = FPaths::Combine(DefaultUrl, InUrl);

	Request->SetURL(Url);
	Request->SetVerb(Inverb);
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(Content);

	if (bUseAuth)
	{
		if (!AuthInfo.bLogin)
		{
			UE_LOG(LogTemp, Warning, TEXT("User is not logged in"));
			Callback.ExecuteIfBound(FAPIResponse{ 1 });
		}
		else
		{
			FString AuthHeader = FString::Printf(TEXT("Bearer %s"), *(AuthInfo.Token));
			Request->SetHeader(TEXT("Authorization"), AuthHeader);
		}
	}

	Request->OnProcessRequestComplete()
		.BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bProcessedSuccessfully)
		{
			if (!bProcessedSuccessfully || !Response.IsValid())
			{
				Callback.ExecuteIfBound(FAPIResponse{ 2 });
				return;
			}

			FString ResponseContent = Response->GetContentAsString();

			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);
			if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
			{
				Callback.ExecuteIfBound(FAPIResponse{ 1 });
				return;
			}

			FAPIResponse OutResponse;
			OutResponse.ErrorCode = JsonObject->GetNumberField(TEXT("errorcode"));
			OutResponse.Data = nullptr;

			if (JsonObject->HasTypedField<EJson::Object>(TEXT("data")))
			{
				OutResponse.Data = JsonObject->GetObjectField(TEXT("data"));
			}

			Callback.ExecuteIfBound(OutResponse);
			return;
		});

	Request->ProcessRequest();
}
