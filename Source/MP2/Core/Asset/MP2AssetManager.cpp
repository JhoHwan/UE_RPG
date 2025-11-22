// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Asset/MP2AssetManager.h"
#include "AbilitySystemGlobals.h"

void UMP2AssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	UAbilitySystemGlobals::Get().InitGlobalData();
}

UMP2AssetManager& UMP2AssetManager::Get()
{
	UMP2AssetManager* This = Cast<UMP2AssetManager>(GEngine->AssetManager);

	checkf(This != nullptr, TEXT("UMP2AssetManager is not registered as an asset manager."));

	return *This;
}
