 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MP2Interactable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMP2Interactable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MP2_API IMP2Interactable
{
	GENERATED_BODY()

public:
	virtual void Interact(class AMP2Character* Interactor) = 0;
	
	virtual FString GetInteractText() const = 0;
};
