// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../MP2ItemData.h"
#include "MP2.h"
#include "MP2GearItemData.generated.h"

/**
 * 
 */

UCLASS()
class MP2_API UMP2GearItemData : public UMP2ItemData
{
	GENERATED_BODY()

protected:
	// 내부 제어용 플래그 (에디터 표시/숨김에 사용)
	UPROPERTY(VisibleAnywhere, Category = "Visual", meta = (AllowPrivateAccess = "true"))
	bool bUseSkeletalMesh = false;

	UPROPERTY(VisibleAnywhere, Category = "Visual", meta = (AllowPrivateAccess = "true"))
	bool bUseStaticMesh = false;

public:
	UMP2GearItemData();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gear")
	EGearSlotType GearSlotType = EGearSlotType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gear", meta = (EditCondition = ""))
	bool bIsOutFitOnly = false;
	
	UPROPERTY(EditAnywhere, Category = "Visual", meta = (EditCondition = "bUseSkeletalMesh", EditConditionHides))
	TObjectPtr<USkeletalMesh> SkeletalMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Visual", meta = (EditCondition = "bUseStaticMesh", EditConditionHides))
	TObjectPtr<UStaticMesh> StaticMesh = nullptr;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	void UpdateMeshUsageFlags();
};
