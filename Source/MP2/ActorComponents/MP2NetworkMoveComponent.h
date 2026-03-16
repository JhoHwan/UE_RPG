// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MP2NetworkMoveComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MP2_API UMP2NetworkMoveComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMP2NetworkMoveComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	void SetTargetMovePath(TArray<FVector> InWaypoints, TArray<uint32> InTimeOffsets);
	FVector GetMoveDestination() const {return MoveWaypoints.Last();}

private:
	bool bIsMoving;

public:
	[[nodiscard]] bool IsMoving() const { return bIsMoving; }

private:
	TArray<FVector> MoveWaypoints;
	TArray<float> MoveArrivalTimes;
	float MoveStartTime;
};
