// Fill out your copyright notice in the Description page of Project Settings.

#include "MP2NetSubsystem.h"
#include "GNSession.h"
#include "ClientPacketHandler.h"
#include "Character/MP2Character.h"
#include "Core/Setting//MP2NetworkObjectSettings.h"
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

void UMP2NetSubsystem::OnRecvPing(uint64 ServerSendTick)
{
	Protocol::CS_PONG PongPkt; 
	PongPkt.set_server_send_tick(ServerSendTick);
	PongPkt.set_client_recv_tick(GetLocalTick());
	PongPkt.set_client_send_tick(GetLocalTick()); 
	RegisterSend(ClientPacketHandler::MakeSendBuffer(PongPkt));
}

void UMP2NetSubsystem::OnRecvTimeSync(int64 ServerOffset, uint32 RTT)
{
	ServerClockOffset.store(ServerOffset);
	CurrentRTT.store(RTT);
	UE_LOG(LogTemp, Log, TEXT("Sync Complete! Offset: %lld, RTT: %u"), ServerOffset, RTT);
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
		Character->IsOwnPlayer = true;
	}
}

void UMP2NetSubsystem::DespawnNetObject(const uint64& ObjectId)
{
	AActor** NetObjectPtr = NetworkObjectMap.Find(ObjectId);
	if (!NetObjectPtr) return;
	
	AActor* NetObject = *NetObjectPtr;
	if (!NetObject) return;
	
	NetObject->Destroy();
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
