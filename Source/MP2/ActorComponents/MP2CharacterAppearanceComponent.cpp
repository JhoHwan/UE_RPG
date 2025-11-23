// Fill out your copyright notice in the Description page of Project Settings.


#include "MP2CharacterAppearanceComponent.h"
#include "Character/MP2HumanCharacterBase.h"
#include "Items/Data/Gear/MP2GearItemData.h"
#include "Items/Data/Appearance/Hair/MP2HairAppearanceItemData.h"

// Sets default values for this component's properties
UMP2CharacterAppearanceComponent::UMP2CharacterAppearanceComponent()
{
}

void UMP2CharacterAppearanceComponent::InitializeParts(const FMP2CharacterPartComponents& InCharacterPartComponents)
{
	GearSlotToMeshComponentMap.Add(EGearSlot::Cloth, InCharacterPartComponents.ClothMesh);
	GearSlotToMeshComponentMap.Add(EGearSlot::Pants, InCharacterPartComponents.PantsMesh);
	GearSlotToMeshComponentMap.Add(EGearSlot::Gloves, InCharacterPartComponents.GlovesMesh);
	GearSlotToMeshComponentMap.Add(EGearSlot::Shoes, InCharacterPartComponents.ShoesMesh);
	//SlotToMeshMap.Add(EGearSlotType::Cape, InCharacterPartComponents.CapeMesh);
	GearSlotToMeshComponentMap.Add(EGearSlot::Cap, InCharacterPartComponents.CapMesh);
	//SlotToMeshMap.Add(EGearSlotType::EarRing, InCharacterPartComponents.EarRingMesh);

	HairMeshComponent = InCharacterPartComponents.HairMesh;
}

void UMP2CharacterAppearanceComponent::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<AMP2HumanCharacterBase>(GetOwner());
	check(Character);

	//Character->OnGearChanged.AddUObject(this, &UMP2CharacterAppearanceComponent::ChangeGearAppearance);
}


void UMP2CharacterAppearanceComponent::ChangeGearAppearance(EGearSlot GearSlot, UMP2GearItemData* GearItemData)
{
	if (!GearItemData)
	{
		ClearGearAppearance(GearSlot);
		return;
	}

	UMeshComponent** TargetMeshComponentPtr = GearSlotToMeshComponentMap.Find(GearSlot);
	if (TargetMeshComponentPtr == nullptr) return;

	UMeshComponent* TargetMeshComponent = *TargetMeshComponentPtr;
	EAppearanceMeshType MeshType = GearItemData->GetMeshType();
	switch (MeshType)
	{
	case EAppearanceMeshType::Skeletal:
		if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(TargetMeshComponent))
		{
			SkeletalMeshComponent->SetSkeletalMesh(GearItemData->SkeletalMesh);
		}
		break;
	case EAppearanceMeshType::Static:
		if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(TargetMeshComponent))
		{
			StaticMeshComponent->SetStaticMesh(GearItemData->StaticMesh);
		}
		break;

	case EAppearanceMeshType::None:
	default:
		break;
	}

	if (GearSlot == EGearSlot::Cap)
	{
		ChangeHairVisibilityMode(GearItemData->GetHairVisibilityMode());

	}
}

void UMP2CharacterAppearanceComponent::ClearGearAppearance(EGearSlot GearSlot)
{
	if (!ensure(GearSlot == EGearSlot::Cap || GearSlot == EGearSlot::EarRing))
		return;

	if (GearSlot == EGearSlot::Cap)
	{
		ChangeHairVisibilityMode(EHairVisibilityMode::Default);
	}

	UMeshComponent** TargetMeshComponentPtr = GearSlotToMeshComponentMap.Find(GearSlot);
	if (!TargetMeshComponentPtr) return;

	if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(*TargetMeshComponentPtr))
	{
		StaticMeshComponent->SetStaticMesh(nullptr);
	}
}

void UMP2CharacterAppearanceComponent::ChangeAppearance(UMP2AppearanceItemData* AppearanceItemData)
{
	if (!AppearanceItemData) return;

	EAppearanceItemType AppearanceItemType = AppearanceItemData->GetAppearanceItemType();
	switch (AppearanceItemType)
	{
	case EAppearanceItemType::Hair:
		ChangeHairMesh(Cast<UMP2HairAppearanceItemData>(AppearanceItemData));
		ApplyHairColor();
		break;
	case EAppearanceItemType::Skin:
		break;
	case EAppearanceItemType::Face:
		break;

	case EAppearanceItemType::None:
		break;
	}
}

void UMP2CharacterAppearanceComponent::ChangeHairColor(FLinearColor NewColor)
{
	if (NewColor == HairColor) return;
	HairColor = NewColor;
	ApplyHairColor();
}

void UMP2CharacterAppearanceComponent::ChangeHairVisibilityMode(EHairVisibilityMode NewHairVisibilityMode)
{
	if (HairVisibilityMode == NewHairVisibilityMode) return;
	HairVisibilityMode = NewHairVisibilityMode;

	ApplyHairMesh();
}

void UMP2CharacterAppearanceComponent::ChangeHairMesh(UMP2HairAppearanceItemData* HairItemData)
{
	if (!HairItemData)
	{
		return;
	}

	UStaticMesh* DefaultMesh = HairItemData->GetDefaultHair().Get();
	if (!DefaultMesh)
	{
		DefaultMesh = HairItemData->GetDefaultHair().LoadSynchronous();
	}

	UStaticMesh* OverrideMesh = HairItemData->GetOverrideHair().Get();
	if (!OverrideMesh)
	{
		OverrideMesh = HairItemData->GetOverrideHair().LoadSynchronous();
	}

	HairMeshMap.Empty();
	HairMeshMap.Add(EHairVisibilityMode::Default, DefaultMesh);
	HairMeshMap.Add(EHairVisibilityMode::Override, OverrideMesh);
	HairMeshMap.Add(EHairVisibilityMode::Hide, nullptr);

	HairMID = nullptr;

	ApplyHairMesh();
}

void UMP2CharacterAppearanceComponent::ApplyHairMesh()
{
	UStaticMesh** HairMeshPtr = HairMeshMap.Find(HairVisibilityMode);
	if (HairMeshPtr)
	{
		HairMeshComponent->SetStaticMesh((*HairMeshPtr));
	}
}

void UMP2CharacterAppearanceComponent::ApplyHairColor()
{
	if (!HairMeshComponent) return;

	UMaterialInterface* Material = HairMeshComponent->GetMaterial(0);
	if (!Material) return;

	UMaterialInstanceDynamic* ExistingMID = Cast<UMaterialInstanceDynamic>(Material);

	if (ExistingMID)
	{
		HairMID = ExistingMID;
	}
	else
	{
		HairMID = HairMeshComponent->CreateAndSetMaterialInstanceDynamic(0);
	}

	HairMID->SetVectorParameterValue(TEXT("HairColor"), HairColor);
}
