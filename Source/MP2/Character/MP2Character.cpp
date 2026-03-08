// Fill out your copyright notice in the Description page of Project Settings.


#include "MP2Character.h"

#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "ActorComponents/MP2NetworkMoveComponent.h"
#include "AI/NavigationSystemBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Network/MP2NetSubsystem.h"

// Sets default values
AMP2Character::AMP2Character()
{
	PrimaryActorTick.bCanEverTick = false;
	NetworkMoveComp = CreateDefaultSubobject<UMP2NetworkMoveComponent>(FName("NetworkMoveComp"));
}

void AMP2Character::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Custom);
	GetGameInstance()->GetSubsystem<UMP2NetSubsystem>()->MyCharacter = this;
}

// Called to bind functionality to input
void AMP2Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMP2Character::MoveToLocationLocally(const FVector& Dest)
{
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys) return;
	
	UNavigationPath* NavPath = NavSys->FindPathToLocationSynchronously(GetWorld(), GetActorLocation(), Dest);
    
	if (NavPath && NavPath->PathPoints.Num() > 0)
	{
		if (NetworkMoveComp)
		{
			NetworkMoveComp->SetTargetMovePath(NavPath->PathPoints, 500.0f);
		}
	}
}

void AMP2Character::OnReceiveServerMovePath(TArray<FVector> ServerWaypoints, int64 ServerStartTime)
{
	if (!NetworkMoveComp || ServerWaypoints.Num() < 2) return;
	
	TArray<int64> ComputedArrivalTimes;
	ComputedArrivalTimes.Reserve(ServerWaypoints.Num());
	int64 AccumulatedTime = ServerStartTime;
	for (int i = 1; i < ServerWaypoints.Num(); i++)
	{
		const float Dist = FVector::Dist2D(ServerWaypoints[i - 1], ServerWaypoints[i]);
		const float DeltaSeconds = Dist / 500.0;
		
		int64 TimeToReach = static_cast<int64>(DeltaSeconds * 1000.0f);
		AccumulatedTime += TimeToReach;
        
		ComputedArrivalTimes.Add(AccumulatedTime);
	}
	
	UMP2NetSubsystem* NetSubsystem = GetGameInstance()->GetSubsystem<UMP2NetSubsystem>();
	if (!NetSubsystem) return;
	int64 ServerNow = NetSubsystem->GetServerTime();
	
	FVector CurrentPos = GetActorLocation();
	FVector PredictedServerPos = ServerWaypoints.Last(); 
	int32 StartIndex = ServerWaypoints.Num() - 1;
	
	if (ServerNow < ServerStartTime)
	{
		PredictedServerPos = ServerWaypoints[0];
		StartIndex = 1;
	}
	else
	{
		// 타임라인 탐색
		for (int32 i = 1; i < ComputedArrivalTimes.Num(); ++i)
		{
			if (ServerNow <= ComputedArrivalTimes[i])
			{
				int64 T0 = ComputedArrivalTimes[i - 1];
				int64 T1 = ComputedArrivalTimes[i];

				// 선형 보간 비율 (0.0 ~ 1.0)
				float Alpha = static_cast<float>(ServerNow - T0) / static_cast<float>(T1 - T0);
				Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);

				PredictedServerPos = FMath::Lerp(ServerWaypoints[i - 1], ServerWaypoints[i], Alpha);
				StartIndex = i;
				break;
			}
		}
	}
	
	float ErrorDist = FVector::Dist2D(CurrentPos, PredictedServerPos);
	TArray<FVector> BlendedPath;

	if (ErrorDist > 200.0f)
	{
		SetActorLocation(PredictedServerPos);
		BlendedPath.Add(PredictedServerPos);
	}
	else
	{
		BlendedPath.Add(CurrentPos);
	}
	
	for (int32 i = StartIndex; i < ServerWaypoints.Num(); ++i)
	{
		if (FVector::Distance(BlendedPath.Last(), ServerWaypoints[i]) > 50.0f)
		{
			BlendedPath.Add(ServerWaypoints[i]);
		}
	}
	
	if (BlendedPath.Num() > 1)
	{
		NetworkMoveComp->SetTargetMovePath(BlendedPath, 500.0f);
	}
}