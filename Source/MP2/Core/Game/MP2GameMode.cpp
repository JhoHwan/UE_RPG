// Fill out your copyright notice in the Description page of Project Settings.


#include "MP2GameMode.h"

#include "Network/MP2NetSubsystem.h"
#include "Network/ClientPacketHandler.h"

void AMP2GameMode::BeginPlay()
{
	Super::BeginPlay();

	UMP2NetSubsystem* GNSubSystem = GetGameInstance()->GetSubsystem<UMP2NetSubsystem>();
	if (!GNSubSystem) return;
	
	Protocol::CS_FIELD_LOADING_COMPLETE pkt;
	pkt.set_is_success(true);
	
	SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
	GNSubSystem->RegisterSend(SendBuffer);
}
