// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MP2.h"
#include "MP2CharacterAppearanceComponent.generated.h"

class AMP2HumanCharacterBase;
class UMP2GearItemData;
class UMP2AppearanceItemData;
class UMP2HairAppearanceItemData;

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
	
	UFUNCTION(BlueprintCallable)
	void ChangeGearAppearance(EGearSlot GearSlot, UMP2GearItemData* GearItemData);

	UFUNCTION(BlueprintCallable)
	void ChangeAppearance(UMP2AppearanceItemData* AppearanceItemData);

	UFUNCTION(BlueprintCallable)
	void ChangeHairColor(FLinearColor NewColor);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;



private:
	void ClearGearAppearance(EGearSlot GearSlot);
	void ChangeHairVisibilityMode(EHairVisibilityMode NewHairVisibilityMode);
	void ChangeHairMesh(UMP2HairAppearanceItemData* AppearanceItemData);
	
	void ApplyHairMesh();
	void ApplyHairColor();

private:
	UPROPERTY()
	AMP2HumanCharacterBase* Character;

	UPROPERTY()
	FMP2CharacterPartComponents CharacterPartComponents;

	UPROPERTY()
	TMap<EGearSlot, UMeshComponent*> GearSlotToMeshComponentMap;

	UPROPERTY()
	UStaticMeshComponent* HairMeshComponent;

	UPROPERTY()
	UMaterialInstanceDynamic* HairMID;

	UPROPERTY()
	TMap<EHairVisibilityMode, UStaticMesh*> HairMeshMap;

	FLinearColor HairColor = FLinearColor::Black;

	EHairVisibilityMode HairVisibilityMode = EHairVisibilityMode::Default;
	EEarRingVisibilityMode EarRingVisibilityMode = EEarRingVisibilityMode::Default;
};

