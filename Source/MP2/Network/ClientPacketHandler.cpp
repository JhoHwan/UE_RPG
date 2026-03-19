#include "ClientPacketHandler.h"

#include "GNSession.h"
#include "MP2NetSubsystem.h"
#include "ActorComponents/MP2NetworkMoveComponent.h"
#include "Character/MP2Character.h"
#include "Core/MP2GameInstance.h"
#include "Core/MP2UISubSystem.h"

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
	UMP2GameInstance* GameInstance = Cast<UMP2GameInstance>(session->GetGameInstance());
	int32 Id = static_cast<int32>(pkt.target_map_id());

	AsyncTask(ENamedThreads::GameThread, [GameInstance, Id]()
	{
		if (GameInstance)
		{
			GameInstance->LoadLevelWithFade(Id);
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
	TArray<uint32> ArrivalTimesOffset;
	uint64 startTime = pkt.start_server_tick();
	Waypoints.Reserve(pkt.waypoints_size());
	ArrivalTimesOffset.Reserve(pkt.waypoints_size());
	
	for (int i = 0; i < pkt.waypoints_size(); i++)
	{
		Waypoints.Emplace(pkt.waypoints(i).pos().x(), pkt.waypoints(i).pos().y(), pkt.waypoints(i).pos().z());
		ArrivalTimesOffset.Add(pkt.waypoints(i).arrival_offset_ms());
	}

	const uint64 ID = pkt.object_id();
	UMP2NetSubsystem* NetSubSystem = session->GetGameInstance()->GetSubsystem<UMP2NetSubsystem>();

	AsyncTask(ENamedThreads::GameThread,
		[NetSubSystem, Waypoints = MoveTemp(Waypoints), ArrivalTimes = MoveTemp(ArrivalTimesOffset), startTime, ID]()
	{
		if (GEngine && GEngine->GetWorldContexts().Num() > 0)
		{
			AActor* NetObject = NetSubSystem->GetNetworkObject(ID);
			if (!NetObject) return;

			// 컴포넌트를 찾아 서버 시간 기반 타임라인 데이터 전달
			if (auto* MoveComp = NetObject->FindComponentByClass<UMP2NetworkMoveComponent>())
			{
				MoveComp->HandleServerMovePath(Waypoints, ArrivalTimes, startTime);
			}
		}
	});
	return true;
}

bool Handle_SC_TIME_SYNC(SessionRef& session, Protocol::SC_TIME_SYNC& pkt)
{
	int64 ServerOffset = pkt.server_offset();
	uint32 RTT = pkt.rtt();

	if (UMP2NetSubsystem* NetSubSystem = session->GetGameInstance()->GetSubsystem<UMP2NetSubsystem>())
	{
		NetSubSystem->OnRecvTimeSync(ServerOffset, RTT);
	}
	return true;
}

bool Handle_SC_PING(SessionRef& session, Protocol::SC_PING& pkt)
{
	if (UMP2NetSubsystem* NetSubSystem = session->GetGameInstance()->GetSubsystem<UMP2NetSubsystem>())
	{
		NetSubSystem->OnRecvPing(pkt.server_send_tick());
	}
	return true;
}
