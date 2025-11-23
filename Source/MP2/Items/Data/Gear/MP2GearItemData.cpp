// Fill out your copyright notice in the Description page of Project Settings.


#include "MP2GearItemData.h"

UMP2GearItemData::UMP2GearItemData()
{
	ItemType = EItemType::Gear;
	bStackable = false;
}

EAppearanceMeshType UMP2GearItemData::GetMeshType(EGearSlot GearSlot)
{
    EAppearanceMeshType MeshType;
    switch (GearSlot)
    {
        // Skeletal Mesh를 사용하는 슬롯들
    case EGearSlot::Cloth:
    case EGearSlot::Gloves:
    case EGearSlot::Pants:
    case EGearSlot::Shoes:
    case EGearSlot::Cape:
        MeshType = EAppearanceMeshType::Skeletal;
        break;

        // Static Mesh를 사용하는 슬롯들
    case EGearSlot::Cap:
    case EGearSlot::EarRing:
        MeshType = EAppearanceMeshType::Static;
        break;

        // 메시가 없는 슬롯들 (Pendant, Belt 등)
    case EGearSlot::Pendant:
    case EGearSlot::Belt:
    case EGearSlot::None:
    default:
        MeshType = EAppearanceMeshType::None;
        break;
    }
    return MeshType;
}

#if WITH_EDITOR
void UMP2GearItemData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    const FName PropertyName = PropertyChangedEvent.Property
        ? PropertyChangedEvent.Property->GetFName()
        : NAME_None;

    // Slot 값이 바뀔 때마다 플래그 갱신
    if (PropertyName == GET_MEMBER_NAME_CHECKED(UMP2GearItemData, GearSlot))
    {
        UpdateMeshUsageFlags();
    }
}
#endif


void UMP2GearItemData::UpdateMeshUsageFlags()
{
    MeshType = UMP2GearItemData::GetMeshType(GearSlot);

    if (MeshType != EAppearanceMeshType::Skeletal)
    {
        SkeletalMesh = nullptr;
    }
    if (MeshType != EAppearanceMeshType::Static)
    {
        StaticMesh = nullptr;
    }
}
