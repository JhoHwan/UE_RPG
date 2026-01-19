// Fill out your copyright notice in the Description page of Project Settings.


#include "GameNetSubsystem.h"
#include "SocketSubsystem.h"
#include "Networking.h"
#include "GNSession.h"
#include "Proto\ClientPacketHandler.h"

#include "GNLogMagro.h"

void UGameNetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ClientPacketHandler::Init();
}

void UGameNetSubsystem::Deinitialize()
{
	DestroySession();
	Super::Deinitialize();
}

bool UGameNetSubsystem::ConnectToGameServer(const FString& IP, int32 Port)
{
	Session = MakeShared<FGNSession>(IP, Port);
	Session->SetConnectHandler(FOnConnectResult::CreateWeakLambda(this,
		[this](bool bResult)
		{
			if (bResult)
			{
				GN_SCREENLOG("Connect Success");
				bConnected = true;
			}
			else
			{
				GN_SCREENLOG("Connect Failed");
				DestroySession();
			}
		}));

	SessionThread = FRunnableThread::Create(Session.Get(), TEXT("Network Thread"));
	if (!SessionThread)
	{
		GN_ERR("SessionThread Create Fail");
	}

	return true;
}

void UGameNetSubsystem::DestroySession()
{
	if (SessionThread)
	{
		SessionThread->Kill(true);

		delete SessionThread;
		SessionThread = nullptr;
	}

	if (Session)
	{
		Session.Reset();
	}
}

// TickableObject 
void UGameNetSubsystem::Tick(float DeltaTime)
{
	static float Time = 0;
	Time += DeltaTime;
	if (Time > 1.0f)
	{
		//GN_SCREENLOG("UGameNetSubsystem::Tick");
		Time = 0;
		for (int i = 0; i < 5; i++)
		{
			Protocol::CS_PING Packet;
			Packet.set_id(i+1);
			auto SendBuffer = ClientPacketHandler::MakeSendBuffer(Packet);
			if (!SendBuffer) return;
			Session->RegisterSend(SendBuffer);
		}
	}
}

bool UGameNetSubsystem::IsTickable() const
{
	if (IsTemplate()) return false;
	if (!Session) return false;

	return bConnected;
}

bool UGameNetSubsystem::IsTickableWhenPaused() const
{
	return true;
}


TStatId UGameNetSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UGameNetSubsystem, STATGROUP_Tickables);
}