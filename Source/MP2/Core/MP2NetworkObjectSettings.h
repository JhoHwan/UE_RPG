// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MP2Character.h"
#include "Engine/DeveloperSettings.h"
#include "MP2NetworkObjectSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, defaultconfig, meta=(DisplayName="Network Object Settings"))
class MP2_API UMP2NetworkObjectSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, config, Category = "Network Spawning")
	TSubclassOf<AMP2Character> PlayerClass;
};
