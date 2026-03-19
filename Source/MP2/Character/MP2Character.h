// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Field/MP2FieldPortal.h"
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

	virtual bool IsLocallyControlled() const override { return IsOwnPlayer; }
	bool IsMoving() const;
	FVector GetMoveDestination();
	
	void SetCurrentInteractable(IMP2Interactable* NewInteractable);
	void ClearCurrentInteractable(IMP2Interactable* OldInteractable);

public:
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void DoInteract();
	
public :
	bool IsOwnPlayer = false;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Network", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMP2NetworkMoveComponent> NetworkMoveComp;

	IMP2Interactable* CurrentInteractable;
};
