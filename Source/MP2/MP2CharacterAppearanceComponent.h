// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MP2.h"
#include "MP2CharacterAppearanceComponent.generated.h"

class AMP2HumanCharacterBase;
class UMP2GearItemData;

USTRUCT(BlueprintType)
struct FMP2CharacterPartComponents
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> HairMesh = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> FaceMesh = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> CapMesh = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> SkinMaterial = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> ClothMesh = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> PantsMesh = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> GlovesMesh = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> ShoesMesh = nullptr;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MP2_API UMP2CharacterAppearanceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMP2CharacterAppearanceComponent();

	void InitializeParts(const FMP2CharacterPartComponents& InCharacterPartComponents);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void ChangeAppearance(UMP2GearItemData* GearItemData);

private:
	AMP2HumanCharacterBase* Character;
	FMP2CharacterPartComponents CharacterPartComponents;

	TMap<EGearSlotType, UMeshComponent*> SlotToMeshMap;
};
