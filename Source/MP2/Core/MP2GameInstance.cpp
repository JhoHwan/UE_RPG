// Fill out your copyright notice in the Description page of Project Settings.


#include "MP2GameInstance.h"

#include "MP2UISubSystem.h"
#include "Character/MP2Character.h"
#include "Kismet/GameplayStatics.h"
#include "Network/ClientPacketHandler.h"
#include "Network/MP2NetSubsystem.h"
#include "Network/Protocol.pb.h"
#include "Network/Struct.pb.h"

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

void UMP2GameInstance::LoadLevelWithFade(FName NextLevelName)
{
	LevelName = NextLevelName;

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
	if (LevelName != NAME_None)
	{
		UGameplayStatics::OpenLevel(this, LevelName);
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





