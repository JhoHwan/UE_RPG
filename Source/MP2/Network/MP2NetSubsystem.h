// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Struct.pb.h"
#include "Subsystems/Subsystem.h"
#include "Tickable.h"
#include "MP2NetSubsystem.generated.h"

class AMP2Character;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnServerConnectionCompleted, bool, bSuccess);

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

	void RegisterSend(TSharedPtr<class FSendBuffer> SendBuffer);
		
	int64 GetServerTime() const
	{
		return static_cast<uint64>(FPlatformTime::Seconds() * 1000.0) + ServerClockOffset.load();
	}
	void RequestTimeSync();
	void OnReceiveTimeSync(const uint64 ClientTick, const uint64 ServerTick);
	
	void SpawnCharacter(const Protocol::PlayerInfo& PlayerInfo, bool IsOwnPlayer = false);
	AActor* GetNetworkObject(uint64 ObjectId);
	
protected:
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual TStatId GetStatId() const override;

private:
	void DestroySession();
	void HandleServerConnectionCompleted(bool bSuccess);

public:
	UPROPERTY(BlueprintAssignable, Category="GameSever")
	FOnServerConnectionCompleted OnServerConnectionCompleted;
	
private:
	TSharedPtr<class FGNSession> Session;
	FRunnableThread* SessionThread;
	bool bRunning = false;
	float SyncTimeAccumulator = 0.0f;

	static constexpr int32 REQUIRED_INITIAL_SYNCS = 5;
	std::atomic<int32> SyncPingsReceived{0};
	std::atomic<int64> AccumulatedOffset{0};
	std::atomic<int64> ServerClockOffset{0};
	
	UPROPERTY(EditAnywhere, Category="Player")
	TSubclassOf<AMP2Character> DefaultPlayerClass;
	
	UPROPERTY()
	TMap<uint64, AActor*> NetworkObjectMap;
};
