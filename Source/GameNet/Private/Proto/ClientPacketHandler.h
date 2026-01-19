#pragma once
#include "Protocol.pb.h"

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
using SessionRef = TSharedPtr<class FGNSession>;
using SendBufferRef = TSharedPtr<class FSendBuffer>;
#include "GameNet.h"
#else
using SessionRef = shared_ptr<class Session>;
using SendBufferRef = shared_ptr<class SendBuffer>;
#endif

using PacketHandlerFunc = std::function<bool(SessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
	PKT_CS_LOGIN = 1000,
	PKT_SC_LOGIN_RESULT = 1001,
	PKT_CS_MOVE = 1002,
	PKT_CS_PING = 1003,
	PKT_SC_PONG = 1004,
};

// Custom Handlers
bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len);
bool Handle_SC_LOGIN_RESULT(SessionRef& session, Protocol::SC_LOGIN_RESULT& pkt);
bool Handle_SC_PONG(SessionRef& session, Protocol::SC_PONG& pkt);

class ClientPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_SC_LOGIN_RESULT] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::SC_LOGIN_RESULT>(Handle_SC_LOGIN_RESULT, session, buffer, len); };
		GPacketHandler[PKT_SC_PONG] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::SC_PONG>(Handle_SC_PONG, session, buffer, len); };
	}

	static bool HandlePacket(SessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::CS_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_CS_LOGIN); }
	static SendBufferRef MakeSendBuffer(Protocol::CS_MOVE& pkt) { return MakeSendBuffer(pkt, PKT_CS_MOVE); }
	static SendBufferRef MakeSendBuffer(Protocol::CS_PING& pkt) { return MakeSendBuffer(pkt, PKT_CS_PING); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, SessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
		SendBufferRef sendBuffer = MakeShared<FSendBuffer>(packetSize);
#else
		SendBufferRef sendBuffer = make_shared<SendBuffer>(packetSize);
#endif
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;

		bool bSuccess = pkt.SerializeToArray(sendBuffer->Buffer() + sizeof(PacketHeader), dataSize);
		if (!bSuccess) return nullptr;

		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};