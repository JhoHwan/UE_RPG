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
	int64 GetServerClockOffset() const {return ServerClockOffset.load();}

	static uint64 GetLocalTick() {return static_cast<uint64>(FPlatformTime::Seconds() * 1000.0f);}
	
	int64 GetServerTime() const
	{
		return GetLocalTick() + ServerClockOffset.load();
	}
	
	void OnRecvPing(uint64 ServerSendTick);
	void OnRecvTimeSync(int64 ServerOffset, uint32 RTT);
	
	void SpawnCharacter(const Protocol::PlayerInfo& PlayerInfo, bool IsOwnPlayer = false);
	void DespawnNetObject(const uint64& ObjectId);
	
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
	
	std::atomic<int64> ServerClockOffset{0};
	std::atomic<uint32> CurrentRTT{0};
	
	UPROPERTY(EditAnywhere, Category="Player")
	TSubclassOf<AMP2Character> DefaultPlayerClass;
	
	UPROPERTY()
	TMap<uint64, AActor*> NetworkObjectMap;
};
