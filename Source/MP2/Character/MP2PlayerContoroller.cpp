// Fill out your copyright notice in the Description page of Project Settings.


#include "MP2PlayerContoroller.h"

#include "ClientPacketHandler.h"
#include "GameNetSubsystem.h"

void AMP2PlayerContoroller::RequestMove(const FVector& Dest)
{
	Protocol::CS_REQUEST_MOVE pkt;
	
	Protocol::Vector3* DestPos = pkt.mutable_pos();
	DestPos->set_x(Dest.X);
	DestPos->set_y(Dest.Y);
	DestPos->set_z(Dest.Z);
	
	SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
	GetGameInstance()->GetSubsystem<UGameNetSubsystem>()->RegisterSend(SendBuffer);
}
