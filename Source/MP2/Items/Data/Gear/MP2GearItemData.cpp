// Fill out your copyright notice in the Description page of Project Settings.


#include "MP2GearItemData.h"

UMP2GearItemData::UMP2GearItemData()
{
	ItemType = EItemType::Gear;
	bStackable = false;
}

#if WITH_EDITOR
void UMP2GearItemData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    const FName PropertyName = PropertyChangedEvent.Property
        ? PropertyChangedEvent.Property->GetFName()
        : NAME_None;

    // Slot 값이 바뀔 때마다 플래그 갱신
    if (PropertyName == GET_MEMBER_NAME_CHECKED(UMP2GearItemData, GearSlotType))
    {
        UpdateMeshUsageFlags();
    }
}
#endif


void UMP2GearItemData::UpdateMeshUsageFlags()
{
    // 기본값 초기화
    bUseSkeletalMesh = false;
    bUseStaticMesh = false;

    switch (GearSlotType)
    {
        // Skeletal Mesh를 사용하는 슬롯들
    case EGearSlotType::Cloth:
    case EGearSlotType::Gloves:
    case EGearSlotType::Pants:
    case EGearSlotType::Shoes:
    case EGearSlotType::Cape:
        bUseSkeletalMesh = true;
        break;

        // Static Mesh를 사용하는 슬롯들
    case EGearSlotType::Cap:
    case EGearSlotType::EarRing:
        bUseStaticMesh = true;
        break;

        // 메시가 없는 슬롯들 (Pendant, Belt 등)
    case EGearSlotType::Pendant:
    case EGearSlotType::Belt:
    case EGearSlotType::None:
    default:
        break;
    }

    if (!bUseSkeletalMesh)
    {
        SkeletalMesh = nullptr;
    }
    if (!bUseStaticMesh)
    {
        StaticMesh = nullptr;
    }
}
