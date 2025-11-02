// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "RPGPlayer.generated.h"

UCLASS()
class RPG_API ARPGPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	ARPGPlayer();

protected:
	virtual void PossessedBy(AController* NewController) override;

public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<class USpringArmComponent> CameraSpring;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<class UCameraComponent> FollowCamera;
protected:
	void Move(const FInputActionValue& Value);

	void Zoom(const FInputActionValue& Value);

protected:
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<class UInputMappingContext> DefaultContext;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<class UInputAction> ZoomAction;

	UPROPERTY(EditAnywhere, Category = Camera)
	float CameraRotateSpeed = 10.0f;

	UPROPERTY(EditAnywhere, Category = Camera)
	float CameraZoomInMax = 30.0f;

	UPROPERTY(EditAnywhere, Category = Camera)
	float CameraZoomOutMax = 2000.0f;

	UPROPERTY(EditAnywhere, Category = Camera)
	float CameraZoomSpeed = 50.0f;

	bool bIsRotatingCamera = false;
};
