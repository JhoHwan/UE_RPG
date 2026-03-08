// Fill out your copyright notice in the Description page of Project Settings.


#include "MP2PlayerContoroller.h"

#include "MP2Character.h"
#include "Network/ClientPacketHandler.h"
#include "Network/MP2NetSubsystem.h"
#include "NavigationSystem.h"

AMP2PlayerContoroller::AMP2PlayerContoroller()
{
}


void AMP2PlayerContoroller::RequestMove(const FVector& Dest)
{
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys) return;
	FNavLocation ProjectedLocation;
	if (NavSys->ProjectPointToNavigation(Dest, ProjectedLocation, FVector(500.f, 500.f, 500.f)))
	{
		FVector FinalDest = ProjectedLocation.Location;
		
		Protocol::CS_REQUEST_MOVE pkt;
	
		Protocol::Vector3* DestPos = pkt.mutable_pos();
		DestPos->set_x(FinalDest.X);
		DestPos->set_y(FinalDest.Y);
		DestPos->set_z(FinalDest.Z);
	
		SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
		GetGameInstance()->GetSubsystem<UMP2NetSubsystem>()->RegisterSend(SendBuffer);

		AMP2Character* MP2Character = Cast<AMP2Character>(GetPawn());
		if (MP2Character)
		{
			MP2Character->MoveToLocationLocally(FinalDest);
		}
	}
}

