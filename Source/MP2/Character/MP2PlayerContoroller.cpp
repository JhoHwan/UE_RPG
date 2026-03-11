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
	if (NavSys->ProjectPointToNavigation(Dest, ProjectedLocation, FVector(100, 100, 50)))
	{
		FVector FinalDest = ProjectedLocation.Location;
		
		AMP2Character* MP2Character = Cast<AMP2Character>(GetPawn());
		float CurrentSpeed = 500.0f;
		if (MP2Character)
		{
			//CurrentSpeed = MP2Character->GetCharacterMovement()->MaxWalkSpeed;
		}

		Protocol::CS_REQUEST_MOVE pkt;
	
		Protocol::Vector3* DestPos = pkt.mutable_pos();
		DestPos->set_x(static_cast<float>(FinalDest.X));
		DestPos->set_y(static_cast<float>(FinalDest.Y));
		DestPos->set_z(static_cast<float>(FinalDest.Z));
		
		//pkt.set_speed(CurrentSpeed);
	
		SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
		GetGameInstance()->GetSubsystem<UMP2NetSubsystem>()->RegisterSend(SendBuffer);

		if (MP2Character)
		{
			MP2Character->MoveToLocationLocally(FinalDest, CurrentSpeed);
		}
	}
}

