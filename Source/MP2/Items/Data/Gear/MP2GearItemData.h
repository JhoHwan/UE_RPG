// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../MP2ItemData.h"
#include "MP2.h"
#include "MP2GearItemData.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EAppearanceMeshType : uint8
{
	None		UMETA(DisplayName = "None"),
	Skeletal	UMETA(DisplayName = "Skeletal"),
	Static		UMETA(DisplayName = "Static"),
};

UCLASS()
class MP2_API UMP2GearItemData : public UMP2ItemData
{
	GENERATED_BODY()

public:
	UMP2GearItemData();

	static EAppearanceMeshType GetMeshType(EGearSlot GearSlot);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	GENERATE_GETTER(EHairVisibilityMode, HairVisibilityMode)
	GENERATE_GETTER(EEarRingVisibilityMode, EarRingVisibilityMode)

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Visual|Mesh")
	EAppearanceMeshType MeshType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual|Cap", meta = (EditCondition = "GearSlot == EGearSlot::Cap", EditConditionHides))
	EHairVisibilityMode HairVisibilityMode = EHairVisibilityMode::Override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual|Cap", meta = (EditCondition = "GearSlot == EGearSlot::Cap", EditConditionHides))
	EEarRingVisibilityMode EarRingVisibilityMode = EEarRingVisibilityMode::Override;

public:
	FORCEINLINE EAppearanceMeshType GetMeshType() { return MeshType; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gear")
	EGearSlot GearSlot = EGearSlot::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gear", meta = (EditCondition = ""))
	bool bIsOutFitOnly = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Visual|Mesh", meta = (EditCondition = "MeshType == EAppearanceMeshType::Skeletal", EditConditionHides))
	TObjectPtr<USkeletalMesh> SkeletalMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Visua|Mesh", meta = (EditCondition = "MeshType == EAppearanceMeshType::Static", EditConditionHides))
	TObjectPtr<UStaticMesh> StaticMesh = nullptr;

private:
	void UpdateMeshUsageFlags();
};
