// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FieldPortal.generated.h"

UCLASS()
class MP2_API AFieldPortal : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFieldPortal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
