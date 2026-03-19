// Fill out your copyright notice in the Description page of Project Settings.


#include "MP2NetworkMoveComponent.h"

#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Network/MP2NetSubsystem.h"


// Sets default values for this component's properties
UMP2NetworkMoveComponent::UMP2NetworkMoveComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called every frame
void UMP2NetworkMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TObjectPtr<ACharacter> Owner = Cast<ACharacter>(GetOwner());

	if (!bIsMoving || MoveWaypoints.Num() < 2 || MoveArrivalTimes.Num() < 2)
	{
		return;
	}

	// 언리얼 월드의 현재 절대 시간 (초)
	float Now = GetWorld()->GetTimeSeconds();

	// 2. 종착지 도착 판정
	if (Now >= MoveArrivalTimes.Last())
	{
		FVector FinalDest = MoveWaypoints.Last();
		FVector CurrentLoc = Owner->GetActorLocation();

		FinalDest.Z = CurrentLoc.Z;

		FVector InterpLoc = FMath::VInterpTo(CurrentLoc, FinalDest, DeltaTime, 15.0f);

		if (FVector::Dist2D(InterpLoc, FinalDest) <= 2.0f)
		{
			Owner->SetActorLocation(FinalDest, false);
			bIsMoving = false;
		}
		else
		{
			Owner->SetActorLocation(InterpLoc, false);
		}

		return;
	}

	// 현재 시간에 맞는 이동 구간 탐색
	for (int32 i = 1; i < MoveArrivalTimes.Num(); ++i)
	{
		if (Now <= MoveArrivalTimes[i])
		{
			const float T0 = MoveArrivalTimes[i - 1];
			const float T1 = MoveArrivalTimes[i];

			if (T1 <= T0) continue;
			const float Ratio = (Now - T0) / (T1 - T0);

			const FVector P0 = MoveWaypoints[i - 1];
			const FVector P1 = MoveWaypoints[i];

			const FVector CurrentPos = FMath::Lerp(P0, P1, Ratio);
			FVector FinalPos = CurrentPos;

			UCapsuleComponent* CapComp = Owner->GetCapsuleComponent();
			if (CapComp)
			{
				const float HalfHeight = CapComp->GetScaledCapsuleHalfHeight();
				float TargetZ = CurrentPos.Z + HalfHeight;

				UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
				if (NavSys)
				{
					FNavLocation ProjectedLocation;
					FVector QueryExtent(50.0f, 50.0f, 500.0f);

					if (NavSys->ProjectPointToNavigation(CurrentPos, ProjectedLocation, QueryExtent))
					{
						TargetZ = ProjectedLocation.Location.Z + HalfHeight;
					}
				}
				FinalPos.Z = FMath::FInterpTo(Owner->GetActorLocation().Z, TargetZ, DeltaTime, 10.0f);
			}

			Owner->SetActorLocation(FinalPos, false);

			FVector Direction = (P1 - P0).GetSafeNormal();
			if (!Direction.IsNearlyZero())
			{
				FRotator TargetRot = Direction.Rotation();
				TargetRot.Pitch = 0.0f;
				TargetRot.Roll = 0.0f;

				FRotator SmoothRot = FMath::RInterpTo(Owner->GetActorRotation(), TargetRot, DeltaTime, 12.0f);
				Owner->SetActorRotation(SmoothRot);
			}

			break;
		}
	}
}

void UMP2NetworkMoveComponent::HandleServerMovePath(TArray<FVector> InWaypoints, TArray<uint32> InTimeOffsets, int64 InServerStartTime)
{
	if (InWaypoints.Num() < 2) return;

	// 기존 AMP2Character::OnReceiveServerMovePath 로직 그대로 이전
	TArray<int64> ComputedArrivalTimes;
	ComputedArrivalTimes.Reserve(InWaypoints.Num());
	int64 AccumulatedTime = InServerStartTime;

	for (int i = 0; i < InTimeOffsets.Num(); i++)
	{
		AccumulatedTime += InTimeOffsets[i];
		ComputedArrivalTimes.Add(AccumulatedTime);
	}

	if (ComputedArrivalTimes.Num() != InWaypoints.Num())
	{
		return;
	}

	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	if (!Owner) return;

	UMP2NetSubsystem* NetSubsystem = Owner->GetGameInstance()->GetSubsystem<UMP2NetSubsystem>();
	if (!NetSubsystem) return;
	int64 ServerNow = NetSubsystem->GetServerTime();

	FVector CurrentPos = Owner->GetActorLocation();
	FVector PredictedServerPos = InWaypoints.Last();
	int32 StartIndex = InWaypoints.Num() - 1;

	if (ServerNow < InServerStartTime)
	{
		PredictedServerPos = InWaypoints[0];
		StartIndex = 1;
	}
	else
	{
		for (int32 i = 1; i < ComputedArrivalTimes.Num(); ++i)
		{
			if (ServerNow <= ComputedArrivalTimes[i])
			{
				int64 T0 = ComputedArrivalTimes[i - 1];
				int64 T1 = ComputedArrivalTimes[i];

				float Alpha = static_cast<float>(ServerNow - T0) / static_cast<float>(T1 - T0);
				Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);

				PredictedServerPos = FMath::Lerp(InWaypoints[i - 1], InWaypoints[i], Alpha);
				StartIndex = i;
				break;
			}
		}
	}

	float ErrorDist = FVector::Dist2D(CurrentPos, PredictedServerPos);
	TArray<FVector> BlendedPath;
	
	if (ErrorDist > 500.0f)
	{
		Owner->SetActorLocation(PredictedServerPos);
		BlendedPath.Add(PredictedServerPos);
	}
	else
	{
		BlendedPath.Add(CurrentPos);
	}

	for (int32 i = StartIndex; i < InWaypoints.Num(); ++i)
	{
		if (FVector::Distance(BlendedPath.Last(), InWaypoints[i]) > 50.0f)
		{
			BlendedPath.Add(InWaypoints[i]);
		}
	}

	if (BlendedPath.Num() > 1)
	{
		// 블렌딩된 경로로 타임라인 재설정
		// Note: 블렌딩 시에도 서버 오프셋을 그대로 활용함 (사용자 기존 방식 유지)
		SetTargetMovePath(MoveTemp(BlendedPath), MoveTemp(InTimeOffsets));
	}
}

void UMP2NetworkMoveComponent::SetTargetMovePath(TArray<FVector> InWaypoints, TArray<uint32> InTimeOffsets)
{
	if (InWaypoints.Num() < 2 || InWaypoints.Num() != InTimeOffsets.Num())
	{
		bIsMoving = false;
		return;
	}

	MoveWaypoints = MoveTemp(InWaypoints);
	MoveArrivalTimes.Empty();
	MoveArrivalTimes.Reserve(MoveWaypoints.Num());

	MoveStartTime = GetWorld()->GetTimeSeconds();

	for (int32 i = 0; i < InTimeOffsets.Num(); ++i)
	{
		float OffsetSec = static_cast<float>(InTimeOffsets[i]) / 1000.0f;
		MoveArrivalTimes.Add(MoveStartTime + OffsetSec);
	}

	bIsMoving = true;
}
