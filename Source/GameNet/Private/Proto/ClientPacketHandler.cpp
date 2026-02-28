#include "ClientPacketHandler.h"
#include "GNLogMagro.h"
PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}

bool Handle_SC_ENTER_GAME_RESULT(SessionRef& session, Protocol::SC_ENTER_GAME_RESULT& pkt)
{
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

bool Handle_SC_PONG(SessionRef& session, Protocol::SC_PONG& pkt)
{
	int32 Id = pkt.id();
	AsyncTask(ENamedThreads::GameThread, [Id]()
		{
			GN_SCREENLOG("Pong %d", Id);
		});
	return true;
}
