#pragma once
#include "CoreMinimal.h"
#include "MP2HttpTypes.generated.h"

DECLARE_DELEGATE_OneParam(FOnHttpRequestComplete, const FAPIResponse&)

USTRUCT()
struct FAPIResponse
{
	GENERATED_BODY()

	int32 ErrorCode = 0;
	TSharedPtr<FJsonObject> Data;
};

struct FAuthInfo
{
	bool bLogin{ false };
	FString Token;
};
