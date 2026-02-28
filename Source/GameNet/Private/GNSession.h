// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Proto\ClientPacketHandler.h"

DECLARE_DELEGATE_OneParam(FOnConnectResult, bool)
DECLARE_DELEGATE_OneParam(FOnDisconnect, const FString&)


/**
 * 
 */
class FGNSession : public FRunnable, public TSharedFromThis<FGNSession>
{
public:
	FGNSession(const FString& IP, int32 Port);
	~FGNSession();

	void SetConnectHandler(const FOnConnectResult& InDelegate)
	{
		OnConnectResult = InDelegate;
	}

	void SetConnectHandler(const FOnDisconnect& InDelegate)
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
	void RegisterSend(SendBufferRef SendBuffer);

	bool IsRunning() const { return bIsRunning; }

private:
	int32 RecvPacket();
	int32 HandlePacket();

	int32 SendPacket();

public:

private:
	FOnConnectResult OnConnectResult;
	FOnDisconnect OnDisconnect;

	FString IP;
	int32 Port;

	FSocket* Socket = nullptr;
	bool bIsRunning = false;
	
	const int32 MaxSendSize = 2048;
	TQueue<SendBufferRef> SendQueue;
	TArray<BYTE> SendBufferChunk;

	TSharedPtr<class FRecvBuffer> RecvBuffer;
};
