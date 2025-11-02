#pragma once
#include "CoreMinimal.h"
#include "RPGHttpTypes.generated.h"

DECLARE_DELEGATE_OneParam(FOnHttpRequestComplete, const FAPIResponse&)

USTRUCT()
struct FAPIResponse
{
	GENERATED_BODY()

	bool Error = false;
	FString Message;
	TSharedPtr<FJsonObject> Data;
};
