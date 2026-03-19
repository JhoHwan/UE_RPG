// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Interface/MP2Interactable.h"
#include "MP2FieldPortal.generated.h"

UCLASS()
class MP2_API AMP2FieldPortal : public AActor, public IMP2Interactable
{
	GENERATED_BODY()

public:
	AMP2FieldPortal();
	
	virtual void Interact(AMP2Character* Interactor) override;
	virtual FString GetInteractText() const override;
	
protected:
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
#if WITH_EDITOR
	// 에디터에서 액터가 처음 생성되거나 복사될 때 호출됨
	virtual void PostActorCreated() override
	{
		Super::PostActorCreated();
        
		// 유효하지 않은 Guid라면 새로 발급
		if (!MyGuid.IsValid())
		{
			MyGuid = FGuid::NewGuid();
		}
	}
    
	// 복사&붙여넣기 했을 때 GUID가 중복되는 것을 방지
	virtual void PostDuplicate(bool bDuplicateForPIE) override
	{
		Super::PostDuplicate(bDuplicateForPIE);
		if (!bDuplicateForPIE)
		{
			MyGuid = FGuid::NewGuid();
		}
	}
#endif

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal Data")
	int32 PortalId = -1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal Data")
	FGuid MyGuid;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal Data")
	FGuid TargetGuid;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class USphereComponent> Collision;
};
