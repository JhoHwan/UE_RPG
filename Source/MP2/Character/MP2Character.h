// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MP2Character.generated.h"

UCLASS()
class MP2_API AMP2Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMP2Character();
	
	virtual void BeginPlay() override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void MoveToLocationLocally(const FVector& Dest);
	void OnReceiveServerMovePath(TArray<FVector> ServerWaypoints, int64 ServerStartTime);
	
	virtual bool IsLocallyControlled() const override { return true; }

private:
	UPROPERTY()
	TObjectPtr<class UMP2NetworkMoveComponent> NetworkMoveComp;
};
