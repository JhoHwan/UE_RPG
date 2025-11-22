// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MP2HumanCharacterBase.generated.h"

class UStaticMeshComponent;
class USkeletalMeshComponent;
class UMP2GearItemData;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGearChanged, UMP2GearItemData*)

UENUM(BlueprintType)
enum class ECharacterPartType : uint8
{
	Hair		UMETA(DisplayName = "Hair"),
	Face		UMETA(DisplayName = "Face"),
	Skin		UMETA(DisplayName = "Skin"),

	Cap			UMETA(DisplayName = "Cap"),
	Cloth		UMETA(DisplayName = "Cloth"),
	Pants		UMETA(DisplayName = "Pants"),
	Gloves		UMETA(DisplayName = "Gloves"),
	Shoes		UMETA(DisplayName = "Shoes")
};



UCLASS()
class MP2_API AMP2HumanCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMP2HumanCharacterBase();

public:
	UFUNCTION(BlueprintCallable)
	void EquipmentGear(UMP2GearItemData* GearItemData);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

public:
	FOnGearChanged OnGearChanged;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Part")
	TObjectPtr<UStaticMeshComponent> HairMesh;

	UPROPERTY(BlueprintReadOnly, Category = "Part")
	TObjectPtr<UStaticMeshComponent> FaceMesh;

	UPROPERTY(BlueprintReadOnly, Category = "Part")
	TObjectPtr<UStaticMeshComponent> CapMesh;

	UPROPERTY(BlueprintReadOnly, Category = "Part")
	TObjectPtr<UMaterialInterface> SkinMaterial;

	UPROPERTY(BlueprintReadOnly, Category = "Part")
	TObjectPtr<USkeletalMeshComponent> ClothMesh;

	UPROPERTY(BlueprintReadOnly, Category = "Part")
	TObjectPtr<USkeletalMeshComponent> PantsMesh;

	UPROPERTY(BlueprintReadOnly, Category = "Part")
	TObjectPtr<USkeletalMeshComponent> GlovesMesh;

	UPROPERTY(BlueprintReadOnly, Category = "Part")
	TObjectPtr<USkeletalMeshComponent> ShoesMesh;

	UPROPERTY(BlueprintReadOnly) 
	TObjectPtr<class UMP2CharacterAppearanceComponent> AppearanceComponent;

};
