// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EGenderType : uint8
{
	Male		UMETA(DisplayName = "Male"),
	Female		UMETA(DisplayName = "Female"),
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	None			UMETA(DisplayName = "None"),
	Gear			UMETA(DisplayName = "Gear"),
	Appearance		UMETA(DisplayName = "Appearance"),
	Consumable		UMETA(DisplayName = "Consumable"),
};

UENUM(BlueprintType)
enum class ERarity : uint8
{
	Normal			UMETA(DisplayName = "Normal"),
	Rare			UMETA(DisplayName = "Rare"),
	Elite			UMETA(DisplayName = "Elite"),
	Excellent		UMETA(DisplayName = "Excellent"),
};

UENUM(BlueprintType)
enum class EGearSlot : uint8
{
	None		UMETA(DisplayName = "None"),
	Cap			UMETA(DisplayName = "Cap"),
	Cloth		UMETA(DisplayName = "Cloth"),
	Gloves		UMETA(DisplayName = "Gloves"),
	Pants		UMETA(DisplayName = "Pants"),
	Shoes		UMETA(DisplayName = "Shoes"),
	EarRing		UMETA(DisplayName = "EarRing"),
	Belt		UMETA(DisplayName = "Belt"),
	Pendant		UMETA(DisplayName = "Pendant"),
	Cape		UMETA(DisplayName = "Cape"),
};

UENUM(BlueprintType)
enum class EHairVisibilityMode : uint8
{
	Default		UMETA(DisplayName = "Default"),
	Override	UMETA(DisplayName = "Override"),
	Hide		UMETA(DisplayName = "Hide"),
};

UENUM(BlueprintType)
enum class EEarRingVisibilityMode : uint8
{
	Default		UMETA(DisplayName = "Default"),
	Override	UMETA(DisplayName = "Override"),
	Hide		UMETA(DisplayName = "Hide"),
};