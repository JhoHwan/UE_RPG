// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/EditableTextBox.h"
#include "RPGLoginEmailTextBox.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API URPGLoginEmailTextBox : public UEditableTextBox
{
	GENERATED_BODY()
	
public:
	URPGLoginEmailTextBox();

protected:
	virtual void SynchronizeProperties() override;

};
