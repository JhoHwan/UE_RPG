// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MP2.h"
#include "MP2ItemData.generated.h"

#define GENERATE_SETTER(Type, Name) \
	FORCEINLINE void Set##Name(Type In##Name) { Name = In##Name;}

#define GENERATE_GETTER(Type, Name) \
	FORCEINLINE Type Get##Name() const { return Name; }

#define GENERATE_SETTER_AND_GETTER(Type, Name)	\
	GENERATE_SETTER(Type, Name)					\
	GENERATE_GETTER(Type, Name)					\

UCLASS(Abstract, BlueprintType)
class MP2_API UMP2ItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, Category = "Item")
	EItemType ItemType = EItemType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText ItemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	ERarity Rarity = ERarity::Normal;


	// 스택 가능 여부
	UPROPERTY(VisibleAnywhere, Category = "Stack")
	bool bStackable = false;

	// 최대 스택 개수 (스택 가능일 경우에만 활성화)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stack", meta = (EditCondition = "bStackable", EditConditionHides, ClampMin = "1"))
	int32 MaxStackCount = 1;

	// UI 아이콘
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trade")
	bool bSellable = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trade", meta = (EditCondition = "bSellable", EditConditionHides, ClampMin = "0"))
	int32 SellPrice = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trade")
	bool bTradeable = true;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		// 모든 아이템은 PrimaryAssetType = "Item"
		return FPrimaryAssetId(TEXT("Item"), GetFName());
	}

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};