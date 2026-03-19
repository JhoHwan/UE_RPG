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
	AMP2Character* MP2Character = Cast<AMP2Character>(GetPawn());
	if (!MP2Character) return;
	
	constexpr double MOVE_REQUEST_MIN_INTERVAL = 0.5;
	constexpr double MOVE_REQUEST_MIN_DIST = 300.0;

	double Now = FApp::GetCurrentTime();
	if (MP2Character->IsMoving() && (Now - LastRequestTime < MOVE_REQUEST_MIN_INTERVAL))
	{
		if (FVector::DistXY(Dest, MP2Character->GetMoveDestination()) <= MOVE_REQUEST_MIN_DIST)
		{
			UE_LOG(LogTemp, Log, TEXT("Move request ignored: Too frequent and same destination."));
			return;
		}
	}

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys) return;
	FNavLocation ProjectedLocation;
	UMP2NetSubsystem* NetSubSystem = GetGameInstance()->GetSubsystem<UMP2NetSubsystem>();
	if (NavSys->ProjectPointToNavigation(Dest, ProjectedLocation, FVector(100, 100, 50)))
	{
		FVector FinalDest = ProjectedLocation.Location;
		float CurrentSpeed = 500.0f;

		Protocol::CS_REQUEST_MOVE pkt;

		Protocol::Vector3* DestPos = pkt.mutable_pos();
		DestPos->set_x(static_cast<float>(FinalDest.X));
		DestPos->set_y(static_cast<float>(FinalDest.Y));
		DestPos->set_z(static_cast<float>(FinalDest.Z));
		pkt.set_client_tick(UMP2NetSubsystem::GetLocalTick());
		
		SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);		NetSubSystem->RegisterSend(SendBuffer);
		LastRequestTime = Now;
		LastRequestDest = FinalDest;

		MP2Character->MoveToLocationLocally(FinalDest);
	}
}

