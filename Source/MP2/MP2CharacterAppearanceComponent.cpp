// Fill out your copyright notice in the Description page of Project Settings.


#include "MP2CharacterAppearanceComponent.h"
#include "Character\MP2HumanCharacterBase.h"
#include "Items\Data\Gear\MP2GearItemData.h"

// Sets default values for this component's properties
UMP2CharacterAppearanceComponent::UMP2CharacterAppearanceComponent()
{
}

void UMP2CharacterAppearanceComponent::InitializeParts(const FMP2CharacterPartComponents& InCharacterPartComponents)
{
	SlotToMeshMap.Add(EGearSlotType::Cloth, InCharacterPartComponents.ClothMesh);
	SlotToMeshMap.Add(EGearSlotType::Pants, InCharacterPartComponents.PantsMesh);
	SlotToMeshMap.Add(EGearSlotType::Gloves, InCharacterPartComponents.GlovesMesh);
	SlotToMeshMap.Add(EGearSlotType::Shoes, InCharacterPartComponents.ShoesMesh);
	//SlotToMeshMap.Add(EGearSlotType::Cape, InCharacterPartComponents.CapeMesh);
	SlotToMeshMap.Add(EGearSlotType::Cap, InCharacterPartComponents.CapMesh);
	//SlotToMeshMap.Add(EGearSlotType::EarRing, InCharacterPartComponents.EarRingMesh);
}

void UMP2CharacterAppearanceComponent::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<AMP2HumanCharacterBase>(GetOwner());
	check(Character);

	Character->OnGearChanged.AddUObject(this, &UMP2CharacterAppearanceComponent::ChangeAppearance);
}

void UMP2CharacterAppearanceComponent::ChangeAppearance(UMP2GearItemData* GearItemData)
{
	EGearSlotType GearSlotType = GearItemData->GearSlotType;
	
	if (UMeshComponent** MeshComponentPtr = SlotToMeshMap.Find(GearSlotType))
	{
		UMeshComponent* MeshComponent = *MeshComponentPtr;
		if (!MeshComponent) return;

		if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(MeshComponent))
		{
			SkeletalMeshComponent->SetSkeletalMesh(GearItemData->SkeletalMesh);
		}
		else if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(MeshComponent))
		{
			StaticMeshComponent->SetStaticMesh(GearItemData->StaticMesh);
		}
	}
}
