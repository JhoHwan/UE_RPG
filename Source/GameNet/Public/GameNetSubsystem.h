// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/Subsystem.h"
#include "Tickable.h"
#include "GameNetSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class GAMENET_API UGameNetSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	bool ConnectToGameServer(const FString& IP, int32 Port);

public:
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual TStatId GetStatId() const override;

private:
	void DestroySession();

private:
	TSharedPtr<class FGNSession> Session;
	FRunnableThread* SessionThread;
	bool bConnected = false;
};
