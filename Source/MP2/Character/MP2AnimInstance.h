// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MP2AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MP2_API UMP2AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY(BlueprintReadOnly, Category="Animation")
	float GroundSpeed = 0.0f;
	
private:
	FVector LastLocation = FVector::ZeroVector;
};
