// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MP2FieldDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class MP2_API UMP2FieldDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Field MetaData")
	FString MapName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Field SpatialData")
	TSoftObjectPtr<UWorld> MapAsset;
	
};
