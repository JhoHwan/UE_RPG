#include "ClientPacketHandler.h"

#include "GNSession.h"
#include "MP2NetSubsystem.h"
#include "Character/MP2Character.h"
#include "google/protobuf/stubs/time.h"
PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}

bool Handle_SC_ENTER_GAME_RESULT(SessionRef& session, Protocol::SC_ENTER_GAME_RESULT& pkt)
{
	bool bSuccess = pkt.success();
	if (bSuccess)
	{
		session->OwnerGameInstance->GetSubsystem<UMP2NetSubsystem>()->RequestTimeSync();
	}
	return false;

}
bool Handle_SC_MOVE_FIELD_FAIL(SessionRef& session, Protocol::SC_MOVE_FIELD_FAIL& pkt)
{
	return false;

}
bool Handle_SC_START_FIELD_LOADING(SessionRef& session, Protocol::SC_START_FIELD_LOADING& pkt)
{
	return false;

}
bool Handle_SC_ENTER_FIELD(SessionRef& session, Protocol::SC_ENTER_FIELD& pkt)
{
	return false;
}
bool Handle_SC_SPAWN_PLAYER(SessionRef& session, Protocol::SC_SPAWN_PLAYER& pkt)
{
	return false;

}
bool Handle_SC_DESPAWN_PLAYER(SessionRef& session, Protocol::SC_DESPAWN_PLAYER& pkt)
{
	return false;
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
		//GN_LOG("[%d]: [%f, %f. %f]", i, Point.x(), Point.y(), Point.z());
	}
	
	AsyncTask(ENamedThreads::GameThread, 
		[Session = session, ServerTime = pkt.start_server_tick() ,Waypoints = MoveTemp(Waypoints)]() mutable  
	{
		if (GEngine && GEngine->GetWorldContexts().Num() > 0)
		{
			UMP2NetSubsystem* NetSubSystem = Session->OwnerGameInstance->GetSubsystem<UMP2NetSubsystem>();
			NetSubSystem->MyCharacter->OnReceiveServerMovePath(MoveTemp(Waypoints), ServerTime);
		}
	});
		
	return false;
}

bool Handle_SC_TIME_SYNC(SessionRef& session, Protocol::SC_TIME_SYNC& pkt)
{
	uint64 ClientTick = pkt.client_tick();
	uint64 ServerTick = pkt.server_tick();

	session->OwnerGameInstance->GetSubsystem<UMP2NetSubsystem>()->OnReceiveTimeSync(ClientTick, ServerTick);

	return true;
}