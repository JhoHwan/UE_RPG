// Fill out your copyright notice in the Description page of Project Settings.


#include "MP2GameInstance.h"

#include "MP2UISubSystem.h"
#include "Character/MP2Character.h"
#include "Field/MP2FieldManifestDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Network/ClientPacketHandler.h"
#include "Network/MP2NetSubsystem.h"
#include "Network/Protocol.pb.h"
#include "Network/Struct.pb.h"
#include "Setting/MP2NetworkObjectSettings.h"

void UMP2GameInstance::Init()
{
	Super::Init();
	
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UMP2GameInstance::OnMapLoaded);
}

void UMP2GameInstance::Shutdown()
{
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	
	Super::Shutdown();
}

void UMP2GameInstance::LoadLevelWithFade(int32 MapId)
{
	const UMP2NetworkObjectSettings* Settings = GetDefault<UMP2NetworkObjectSettings>();
	if (Settings->FieldManifestData.IsNull()) return;
	
	UMP2FieldManifestDataAsset* Manifest = Settings->FieldManifestData.LoadSynchronous();
	if (!Manifest)
	{
		UE_LOG(LogTemp, Error, TEXT("FieldManifestData is not set in NetworkObjectSettings."));
		return;
	}

	UMP2FieldDataAsset** FieldDataPtr = Manifest->FieldDats.Find(MapId);
	if (!FieldDataPtr || !(*FieldDataPtr) || (*FieldDataPtr)->MapAsset.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid FieldId: %d"), MapId);
		return;
	}
	
	TargetMapAsset = (*FieldDataPtr)->MapAsset;
	
	UMP2UISubSystem* UISubSystem = GetSubsystem<UMP2UISubSystem>();
	if (UISubSystem)
	{
		UISubSystem->OnFadeOutCompletedDispatcher.AddDynamic(this, &UMP2GameInstance::ExecuteLevelLoad);
		
		UISubSystem->StartFadeOut();
	}
	else
	{
		ExecuteLevelLoad();
	}
}

void UMP2GameInstance::ExecuteLevelLoad()
{
	if (!TargetMapAsset.IsNull())
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, TargetMapAsset);
	}
}

void UMP2GameInstance::OnMapLoaded(UWorld* LoadedWorld)
{
	if (LoadedWorld && LoadedWorld->IsGameWorld() && LoadedWorld == GetWorld())
	{
		auto* UISubSystem = GetSubsystem<UMP2UISubSystem>();
		UISubSystem->AddFadeToViewPort();
		
		UMP2NetSubsystem* NetSubSystem = GetSubsystem<UMP2NetSubsystem>();
		if (NetSubSystem)
		{
			Protocol::CS_FIELD_LOADING_COMPLETE pkt;
			pkt.set_is_success(true);
			
			NetSubSystem->RegisterSend(ClientPacketHandler::MakeSendBuffer(pkt));
		}
	}
}





