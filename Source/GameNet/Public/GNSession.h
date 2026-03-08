// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class FSendBuffer;
class FGNSession;

DECLARE_DELEGATE_ThreeParams(FOnRecvPacket, TSharedPtr<FGNSession>&, BYTE*, int32)
DECLARE_DELEGATE_OneParam(FOnConnect, bool)
DECLARE_DELEGATE_OneParam(FOnDisconnect, const FString&)


/**
 * 
 */
class GAMENET_API FGNSession : public FRunnable, public TSharedFromThis<FGNSession>
{
public:
	FGNSession(const FString& IP, int32 Port);
	virtual ~FGNSession() override;

	void SetConnectHandler(const FOnConnect& InDelegate)
	{
		OnConnectResult = InDelegate;
	}

	void SetDisConnectHandler(const FOnDisconnect& InDelegate)
	{
		OnDisconnect = InDelegate;
	}

public:
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

public:
	bool TryConnect();
	void Disconnect();
	void RegisterSend(TSharedPtr<FSendBuffer> SendBuffer);

	bool IsRunning() const { return bIsRunning; }

	FOnRecvPacket OnRecvPacket;
private:
	int32 RecvPacket();
	int32 HandlePacket();

	int32 SendPacket();

public:
	TWeakObjectPtr<UGameInstance> OwnerGameInstance;
	
private:
	FOnConnect OnConnectResult;
	FOnDisconnect OnDisconnect;

	FString IP;
	int32 Port;

	FSocket* Socket = nullptr;
	bool bIsRunning = false;
	
	const int32 MaxSendSize = 2048;
	TQueue<TSharedPtr<FSendBuffer>> SendQueue;
	TArray<BYTE> SendBufferChunk;

	TSharedPtr<class FRecvBuffer> RecvBuffer;
};
