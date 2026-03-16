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

	// 서버에서 받은 경로 처리 (기존 AMP2Character 로직 그대로 이전)
	void HandleServerMovePath(TArray<FVector> InWaypoints, TArray<uint32> InTimeOffsets, int64 InServerStartTime);

	// 타임라인 데이터 설정
	void SetTargetMovePath(TArray<FVector> InWaypoints, TArray<uint32> InTimeOffsets);

	bool IsMoving() const { return bIsMoving; }
	FVector GetMoveDestination() const { return MoveWaypoints.Num() > 0 ? MoveWaypoints.Last() : FVector::ZeroVector; }

private:
	bool bIsMoving = false;
	TArray<FVector> MoveWaypoints;
	TArray<float> MoveArrivalTimes; // 로컬 초(s) 단위 시간
	float MoveStartTime;
};
