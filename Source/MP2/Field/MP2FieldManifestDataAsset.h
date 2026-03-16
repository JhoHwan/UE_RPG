// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MP2FieldDataAsset.h"
#include "Engine/DataAsset.h"
#include "MP2FieldManifestDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class MP2_API UMP2FieldManifestDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal Data")
	TMap<int32, UMP2FieldDataAsset*> FieldDats;
};
