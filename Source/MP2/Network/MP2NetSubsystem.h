// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/Subsystem.h"
#include "Tickable.h"
#include "MP2NetSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class MP2_API UMP2NetSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	bool ConnectToGameServer(const FString& IP, int32 Port);

public:
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual TStatId GetStatId() const override;

	void RegisterSend(TSharedPtr<class FSendBuffer> SendBuffer);

	UPROPERTY()
	TObjectPtr<class AMP2Character> MyCharacter;
	
	int64 GetServerTime() const
	{
		return static_cast<uint64>(FPlatformTime::Seconds() * 1000.0) + ServerClockOffset.load();
	}
	void RequestTimeSync();
	void OnReceiveTimeSync(const uint64 ClientTick, const uint64 ServerTick);
	
private:
	void DestroySession();

private:
	TSharedPtr<class FGNSession> Session;
	FRunnableThread* SessionThread;
	bool bRunning = false;
	

	static constexpr int32 REQUIRED_INITIAL_SYNCS = 5;
	std::atomic<int32> SyncPingsReceived{0};
	std::atomic<int64> AccumulatedOffset{0};
	
	std::atomic<int64> ServerClockOffset{0};
};
