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

	if (NavPath && NavPath->PathPoints.Num() > 1)
	{
		float Speed = 500.0f; // 캐릭터 이동 속도

		TArray<uint32> TimeOffsets;
		TimeOffsets.Reserve(NavPath->PathPoints.Num());
		TimeOffsets.Add(0);

		for (int32 i = 1; i < NavPath->PathPoints.Num(); ++i)
		{
			float Dist = FVector::Distance(NavPath->PathPoints[i - 1], NavPath->PathPoints[i]);
			uint32 TimeMs = FMath::RoundToInt((Dist / Speed) * 1000.0f);
			TimeOffsets.Add(TimeMs);
		}

		if (NetworkMoveComp)
		{
			NetworkMoveComp->SetTargetMovePath(NavPath->PathPoints, TimeOffsets);
		}
	}
}

bool AMP2Character::IsMoving() const
{
	return NetworkMoveComp ? NetworkMoveComp->IsMoving() : false;
}

FVector AMP2Character::GetMoveDestination()
{
	return NetworkMoveComp ? NetworkMoveComp->GetMoveDestination() : FVector::ZeroVector;
}
