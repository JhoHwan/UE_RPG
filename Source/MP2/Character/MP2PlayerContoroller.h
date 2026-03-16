// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MP2PlayerContoroller.generated.h"

/**
 * 
 */
UCLASS()
class MP2_API AMP2PlayerContoroller : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMP2PlayerContoroller();
	
	UFUNCTION(BlueprintCallable)
	void RequestMove(const FVector& Dest);

private:
	double LastRequestTime = 0.0;
	FVector LastRequestDest = FVector::ZeroVector;
};
