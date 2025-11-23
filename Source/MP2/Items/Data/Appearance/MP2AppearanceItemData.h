// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Data/MP2ItemData.h"
#include "MP2AppearanceItemData.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EAppearanceItemType : uint8
{
	None		UMETA(DisplayName = "None"),
	Hair		UMETA(DisplayName = "Hair"),
	Skin		UMETA(DisplayName = "Skin"),
	Face		UMETA(DisplayName = "Face"),
};

UCLASS(Abstract)
class MP2_API UMP2AppearanceItemData : public UMP2ItemData
{
	GENERATED_BODY()
	
public:
	UMP2AppearanceItemData();

	GENERATE_GETTER(EAppearanceItemType, AppearanceItemType)

protected:
	UPROPERTY(VisibleDefaultsOnly, Category = "Appearance")
	EAppearanceItemType AppearanceItemType = EAppearanceItemType::None;
};
