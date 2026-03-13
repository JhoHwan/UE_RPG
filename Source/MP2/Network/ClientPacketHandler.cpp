#include "ClientPacketHandler.h"

#include "GNSession.h"
#include "MP2NetSubsystem.h"
#include "Character/MP2Character.h"
#include "Core/MP2GameInstance.h"
#include "Core/MP2UISubSystem.h"
#include "google/protobuf/stubs/time.h"
PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}

bool Handle_SC_ENTER_GAME_RESULT(SessionRef& session, Protocol::SC_ENTER_GAME_RESULT& pkt)
{
	bool bSuccess = pkt.success();
	if (!bSuccess)
	{

	}
	return true;
}
bool Handle_SC_MOVE_FIELD_FAIL(SessionRef& session, Protocol::SC_MOVE_FIELD_FAIL& pkt)
{
	return false;

}
bool Handle_SC_START_FIELD_LOADING(SessionRef& session, Protocol::SC_START_FIELD_LOADING& pkt)
{
	session->GetGameInstance()->GetSubsystem<UMP2NetSubsystem>()->RequestTimeSync();
	UMP2GameInstance* GameInstance = Cast<UMP2GameInstance>(session->GetGameInstance());

	AsyncTask(ENamedThreads::GameThread, [GameInstance]()
	{
		if (GameInstance)
		{
			// TODO : 레벨 로드 하드코딩 됨 
			GameInstance->LoadLevelWithFade("Test_Map_0");
		}
	});
	return true;
}
bool Handle_SC_ENTER_FIELD(SessionRef& session, Protocol::SC_ENTER_FIELD& pkt)
{
	UMP2NetSubsystem* NetSubSystem = session->GetGameInstance()->GetSubsystem<UMP2NetSubsystem>();
	AsyncTask(ENamedThreads::GameThread, [NetSubSystem, pkt]()
	{
		NetSubSystem->SpawnCharacter(pkt.my_info(), true);
		
		UMP2UISubSystem* UISubsystem = NetSubSystem->GetGameInstance()->GetSubsystem<UMP2UISubSystem>();
		if (UISubsystem)
		{
			UISubsystem->StartFadeIn();
		}
	});
	
	return true;
}
bool Handle_SC_SPAWN_PLAYER(SessionRef& session, Protocol::SC_SPAWN_PLAYER& pkt)
{
	UMP2NetSubsystem* NetSubSystem = session->GetGameInstance()->GetSubsystem<UMP2NetSubsystem>();
	AsyncTask(ENamedThreads::GameThread, [pkt = pkt, NetSubSystem]()
	{
		for (int i = 0; i < pkt.info_size(); i++)
		{
			NetSubSystem->SpawnCharacter(pkt.info(i));
		}
	});
	
	return true;

}
bool Handle_SC_DESPAWN_PLAYER(SessionRef& session, Protocol::SC_DESPAWN_PLAYER& pkt)
{
	uint64 ObjectId = pkt.player_id();
	UMP2NetSubsystem* NetSubsystem = session->GetGameInstance()->GetSubsystem<UMP2NetSubsystem>();
	AsyncTask(ENamedThreads::GameThread, [NetSubsystem, ObjectId]()
	{
		NetSubsystem->DespawnNetObject(ObjectId);
	});
	
	return true;
}

bool Handle_SC_MOVE_PATH(SessionRef& session, Protocol::SC_MOVE_PATH& pkt)
{
	if (pkt.waypoints_size() < 2) return false;

	TArray<FVector> Waypoints;
	Waypoints.Reserve(pkt.waypoints_size());
	
	for (int i = 0; i < pkt.waypoints_size(); i++)
	{
		const Protocol::Vector3& Point = pkt.waypoints(i);
		Waypoints.Add({Point.x(), Point.y(), Point.z()});
	}

	const uint64 ID = pkt.object_id();
	//const float Speed = pkt.speed();
	UMP2NetSubsystem* NetSubSystem = session->GetGameInstance()->GetSubsystem<UMP2NetSubsystem>();
	
	//TODO : 캐릭터 속도 정보 하드 코딩 됨
	AsyncTask(ENamedThreads::GameThread, 
		[NetSubSystem, ServerTime = pkt.start_server_tick() ,Waypoints = MoveTemp(Waypoints), ID, Speed = 500]() mutable  
	{
		if (GEngine && GEngine->GetWorldContexts().Num() > 0)
		{
			auto* NetObject = NetSubSystem->GetNetworkObject(ID);
			if (!NetObject) return;
			if (AMP2Character* Character = Cast<AMP2Character>(NetObject))
			{
				Character->OnReceiveServerMovePath(MoveTemp(Waypoints), ServerTime, Speed);
			}
		}
	});
		
	return false;
}

bool Handle_SC_TIME_SYNC(SessionRef& session, Protocol::SC_TIME_SYNC& pkt)
{
	uint64 ClientTick = pkt.client_tick();
	uint64 ServerTick = pkt.server_tick();

	session->GetGameInstance()->GetSubsystem<UMP2NetSubsystem>()->OnReceiveTimeSync(ClientTick, ServerTick);

	return true;
}