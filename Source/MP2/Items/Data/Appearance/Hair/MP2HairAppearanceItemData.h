// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Data/Appearance/MP2AppearanceItemData.h"
#include "MP2HairAppearanceItemData.generated.h"

/**
 * 
 */
UCLASS()
class MP2_API UMP2HairAppearanceItemData : public UMP2AppearanceItemData
{
	GENERATED_BODY()
	
public:
	UMP2HairAppearanceItemData();

	GENERATE_GETTER(TSoftObjectPtr<UStaticMesh>, DefaultHair)
	GENERATE_GETTER(TSoftObjectPtr<UStaticMesh>, OverrideHair)

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual|Hair")
	TSoftObjectPtr<UStaticMesh> DefaultHair;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual|Hair")
	TSoftObjectPtr<UStaticMesh> OverrideHair;
};
