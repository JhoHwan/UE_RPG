#include "ClientPacketHandler.h"
#include "GNLogMagro.h"
PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}

bool Handle_SC_LOGIN_RESULT(SessionRef& session, Protocol::SC_LOGIN_RESULT& pkt)
{
	return false;
}

bool Handle_SC_PONG(SessionRef& session, Protocol::SC_PONG& pkt)
{
	int32 Id = pkt.id();
	AsyncTask(ENamedThreads::GameThread, [Id]()
		{
			GN_LOG("Pong %d", Id);
		});
	return true;
}
