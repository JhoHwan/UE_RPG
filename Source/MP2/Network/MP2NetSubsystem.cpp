// Fill out your copyright notice in the Description page of Project Settings.

#include "MP2NetSubsystem.h"
#include "GNSession.h"
#include "ClientPacketHandler.h"
#include "Character/MP2Character.h"
#include "Core/MP2NetworkObjectSettings.h"
#include "Components/CapsuleComponent.h"

void UMP2NetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ClientPacketHandler::Init();
	
	const UMP2NetworkObjectSettings* Settings = GetDefault<UMP2NetworkObjectSettings>();
	if (Settings->PlayerClass)
	{
		DefaultPlayerClass = Settings->PlayerClass;
	}
}

void UMP2NetSubsystem::Deinitialize()
{
	DestroySession();
	Super::Deinitialize();
}

bool UMP2NetSubsystem::ConnectToGameServer(const FString& IP, int32 Port)
{
	if (bRunning) return false;
	bRunning = true;

	Session = MakeShared<FGNSession>(IP, Port, GetGameInstance());
	Session->OnRecvPacket.BindLambda([](SessionRef& Session, BYTE* buffer, int32 length)
	{
		bool result = ClientPacketHandler::HandlePacket(Session, buffer, length);
	});
	
	FOnConnect OnConnect;
	OnConnect.BindUObject(this, &UMP2NetSubsystem::HandleServerConnectionCompleted);
	Session->SetConnectHandler(OnConnect);

	SessionThread = FRunnableThread::Create(Session.Get(), TEXT("Network Thread"));
	if (!SessionThread)
	{
		return false;
	}

	return true;
}

void UMP2NetSubsystem::RequestTimeSync()
{
	Protocol::CS_TIME_SYNC Packet;
	Packet.set_client_tick(static_cast<uint64>(FPlatformTime::Seconds() * 1000.0));
	
	RegisterSend(ClientPacketHandler::MakeSendBuffer(Packet));
}

void UMP2NetSubsystem::OnReceiveTimeSync(const uint64 ClientTick, const uint64 ServerTick)
{
	// T1: 내가 아까 패킷을 보냈을 때의 내 시간
	int64 T1 = static_cast<int64>(ClientTick);
    
	// T2: 서버가 패킷을 받았을 때의 서버 시간
	int64 T2 = static_cast<int64>(ServerTick);
    
	// T4: 지금 응답을 받은 시점의 내 시간
	int64 T4 = static_cast<int64>(FPlatformTime::Seconds() * 1000.0);
    
	// RTT 및 Latency 계산
	int64 RTT = T4 - T1;
	int64 Latency = RTT / 2;
    
	// Offset 계산
	int64 NewOffset = T2 - (T1 + Latency);
	int32 CurrentCount = SyncPingsReceived.load();
	if (CurrentCount < REQUIRED_INITIAL_SYNCS)
	{
		AccumulatedOffset.fetch_add(NewOffset);
		CurrentCount = SyncPingsReceived.fetch_add(1) + 1;
		if (CurrentCount == REQUIRED_INITIAL_SYNCS)
		{
			int64 AverageOffset = AccumulatedOffset.load() / REQUIRED_INITIAL_SYNCS;
			ServerClockOffset.store(AverageOffset);
		}
		else
		{
			RequestTimeSync();
		}
		return;
	}
	
	int64 CurrentOffset = ServerClockOffset.load();
	int64 SmoothedOffset = static_cast<int64>((CurrentOffset * 0.9f) + (NewOffset * 0.1f));
	ServerClockOffset.store(SmoothedOffset);
	UE_LOG(LogTemp, Log, TEXT("ClockOffset %lld"), ServerClockOffset.load())
}

void UMP2NetSubsystem::SpawnCharacter(const Protocol::PlayerInfo& PlayerInfo, bool IsOwnPlayer)
{
	UWorld* World = GetWorld();
	if (!World) return;

	int64 id = PlayerInfo.object_info().id();
	Protocol::Vector3 pos = PlayerInfo.object_info().pos();
	FVector SpawnLocation {pos.x(), pos.y(), pos.z()};
	FRotator SpawnRotator = FRotator::ZeroRotator;

	if (!DefaultPlayerClass) return;
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	AMP2Character* Character = World->SpawnActor<AMP2Character>(DefaultPlayerClass, SpawnLocation, SpawnRotator, SpawnParams);
	if (!Character) return;
	
	if (UCapsuleComponent* Capsule = Character->GetCapsuleComponent())
	{
		SpawnLocation.Z += Capsule->GetScaledCapsuleHalfHeight() / 2;
	}
	
	Character->SetActorLocation(SpawnLocation);
	NetworkObjectMap.Add(id, Character);
	
	if (IsOwnPlayer)
	{
		APlayerController* Controller = GetWorld()->GetFirstPlayerController();
		if (!Controller) return;
	
		Controller->Possess(Character);
	}
}

AActor* UMP2NetSubsystem::GetNetworkObject(uint64 ObjectId)
{
	AActor** ActorPtr = NetworkObjectMap.Find(ObjectId);
	if (ActorPtr == nullptr) return nullptr;
	return *ActorPtr;
}

void UMP2NetSubsystem::DestroySession()
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

	bRunning = false;
}

void UMP2NetSubsystem::HandleServerConnectionCompleted(bool bSuccess)
{
	if (bSuccess == true)
	{
		Protocol::CS_REQ_ENTER_GAME Pkt;
				
		Session->RegisterSend(ClientPacketHandler::MakeSendBuffer(Pkt));
	}
	else
	{
		AsyncTask(ENamedThreads::GameThread, [this]()
		{
			DestroySession();
		});
	}
	
	if (OnServerConnectionCompleted.IsBound())
	{
		OnServerConnectionCompleted.Broadcast(bSuccess);
	}
}

// TickableObject 
void UMP2NetSubsystem::Tick(float DeltaTime)
{
	SyncTimeAccumulator += DeltaTime;
	
	if (SyncTimeAccumulator > 10.0f)
	{
		SyncTimeAccumulator = 0;
		RequestTimeSync();
	}
}

bool UMP2NetSubsystem::IsTickable() const
{
	if (IsTemplate()) return false;
	if (!Session) return false;

	return Session->IsRunning();
}

bool UMP2NetSubsystem::IsTickableWhenPaused() const
{
	return true;
}


TStatId UMP2NetSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UGameNetSubsystem, STATGROUP_Tickables);
}

void UMP2NetSubsystem::RegisterSend(TSharedPtr<FSendBuffer> SendBuffer)
{
	
	if (!bRunning) return;
	Session->RegisterSend(MoveTemp(SendBuffer));
}
