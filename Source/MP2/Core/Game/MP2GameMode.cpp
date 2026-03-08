// Fill out your copyright notice in the Description page of Project Settings.


#include "MP2GameMode.h"

#include "GameNetSubsystem.h"
#include "ClientPacketHandler.h"

void AMP2GameMode::BeginPlay()
{
	Super::BeginPlay();

	UGameNetSubsystem* GNSubSystem = GetGameInstance()->GetSubsystem<UGameNetSubsystem>();
	if (!GNSubSystem) return;
	
	Protocol::CS_FIELD_LOADING_COMPLETE pkt;
	pkt.set_is_success(true);
	
	SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
	GNSubSystem->RegisterSend(SendBuffer);
}
