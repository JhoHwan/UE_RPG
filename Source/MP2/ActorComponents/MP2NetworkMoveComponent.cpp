// Fill out your copyright notice in the Description page of Project Settings.


#include "MP2NetworkMoveComponent.h"

#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"


// Sets default values for this component's properties
UMP2NetworkMoveComponent::UMP2NetworkMoveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called every frame
void UMP2NetworkMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	TObjectPtr<ACharacter> Owner = Cast<ACharacter>(GetOwner());
	
	// 1. 방어적 프로그래밍: 이동 중이 아니거나 데이터가 꼬였으면 즉시 리턴
    if (!bIsMoving || MoveWaypoints.Num() < 2 || MoveArrivalTimes.Num() < 2)
    {
        return;
    }

    // 언리얼 월드의 현재 절대 시간
    float Now = GetWorld()->GetTimeSeconds();

    // 2. 종착지 도착 판정: 현재 시간이 마지막 도착 예정 시간을 지났다면?
	if (Now >= MoveArrivalTimes.Last())
	{
		// 서버가 준 도착 지점의 X, Y는 칼같이 가져오되, 
		FVector FinalDest = MoveWaypoints.Last();
		FVector CurrentLoc = Owner->GetActorLocation();
        
		// Z(높이)는 방금 전까지 예쁘게 맞춰둔 내 캐릭터의 현재 높이로 덮어쓰기 (덜컥거림 방지)
		FinalDest.Z = CurrentLoc.Z;

		// [Soft Landing] 즉시 스냅 대신 부드럽게 이동 (수치가 클수록 더 빠르게 붙음)
		FVector InterpLoc = FMath::VInterpTo(CurrentLoc, FinalDest, DeltaTime, 15.0f);

		// 목적지에 거의 다 왔다면 (거리 2.0 미만) 최종 고정 및 이동 종료
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

            // 0으로 나누기 방지
            if (T1 <= T0) continue; 
            // 진행률 (Ratio: 0.0 ~ 1.0)
            const float Ratio = (Now - T0) / (T1 - T0);

            const FVector P0 = MoveWaypoints[i - 1];
            const FVector P1 = MoveWaypoints[i];

            // --------------------------------------------------
            // ★ A. 위치 보간 (서버의 절대 시간에 종속됨)
            // --------------------------------------------------
        	const FVector CurrentPos = FMath::Lerp(P0, P1, Ratio);
        	FVector FinalPos = CurrentPos;

        	UCapsuleComponent* CapComp = Owner->GetCapsuleComponent();
        	if (CapComp)
        	{
        		const float HalfHeight = CapComp->GetScaledCapsuleHalfHeight();
        		float TargetZ = CurrentPos.Z + HalfHeight; // 목표 Z 높이 (기본값)

        		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
        		if (NavSys)
        		{
        			FNavLocation ProjectedLocation;
        			FVector QueryExtent(50.0f, 50.0f, 500.0f); 

        			if (NavSys->ProjectPointToNavigation(CurrentPos, ProjectedLocation, QueryExtent))
        			{
        				TargetZ = ProjectedLocation.Location.Z + HalfHeight; // 네비메쉬 바닥 기준 목표 높이
        			}
        		}


        		// 현재 내 캐릭터의 Z 위치에서 -> TargetZ를 향해 DeltaTime에 맞춰 부드럽게 따라감
        		// 마지막 인자(15.0f)가 서스펜션의 강도야. (수치가 작을수록 더 부드럽지만 반응이 느림)
        		FinalPos.Z = FMath::FInterpTo(Owner->GetActorLocation().Z, TargetZ, DeltaTime, 10.0f);
        	}


        	// 두 번째 인자인 bSweep을 false로 주면, 언리얼 물리 엔진이 이동 중에 
        	// 바닥이나 벽에 긁히는 걸 계산하지 않고 깔끔하게 '텔레포트' 시켜버림. (중력 싸움 방지)
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

void UMP2NetworkMoveComponent::SetTargetMovePath(TArray<FVector> InWaypoints, float Speed)
{
	// 점이 2개 미만(출발점조차 없거나 제자리)이면 이동 취소
	if (InWaypoints.Num() < 2) 
	{
		bIsMoving = false;
		return;
	}
	
	MoveWaypoints = MoveTemp(InWaypoints);

	MoveArrivalTimes.Empty();
	MoveArrivalTimes.Reserve(MoveWaypoints.Num());
	
	float CurrentTime = GetWorld()->GetTimeSeconds();
    
	MoveStartTime = CurrentTime;
	MoveArrivalTimes.Add(CurrentTime);

	float AccumulatedTime = CurrentTime;

	// 3. 서버랑 똑같이 선형 탐색하면서 구간별 도착 시간 계산
	for (int32 i = 1; i < MoveWaypoints.Num(); ++i)
	{
		// 언리얼 FVector의 Distance 함수로 두 점 사이의 유클리디안 거리 계산
		float Dist = FVector::Distance(MoveWaypoints[i - 1], MoveWaypoints[i]);
		
		float Seconds = Dist / Speed;
        
		AccumulatedTime += Seconds;
		MoveArrivalTimes.Add(AccumulatedTime);
	}
	
	bIsMoving = true;
}

