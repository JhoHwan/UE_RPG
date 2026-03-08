// Fill out your copyright notice in the Description page of Project Settings.


#include "MP2AnimInstance.h"

void UMP2AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	if (AActor* Owner = GetOwningActor())
	{
		LastLocation = Owner->GetActorLocation();
	}
}

void UMP2AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	AActor* Owner = GetOwningActor();
	if (!Owner || DeltaSeconds <= 0.0f) return;
	
	FVector CurrentLocation = Owner->GetActorLocation();
	
	float ActualSpeed = FVector::Dist2D(LastLocation, CurrentLocation) / DeltaSeconds;
	
	GroundSpeed = FMath::FInterpTo(GroundSpeed, ActualSpeed, DeltaSeconds, 15.0f);
	LastLocation = CurrentLocation;
}
