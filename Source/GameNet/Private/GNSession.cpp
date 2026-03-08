// Fill out your copyright notice in the Description page of Project Settings.


#include "GNSession.h"
#include "Networking.h"
#include "GNLogMagro.h"
#include "RecvBuffer.h"

FGNSession::FGNSession(const FString& IP, int32 Port) 
	: IP(IP), Port(Port), 
	RecvBuffer(MakeShared<FRecvBuffer>(0x1000))
{
	SendBufferChunk.Reserve(MaxSendSize); 
}

FGNSession::~FGNSession()
{
	if (Socket)
	{
		Exit();
	}
}


uint32 FGNSession::Run()
{
	GN_LOG("Session Start Run");

	bool bConnected = TryConnect();
	TWeakPtr<FGNSession> WeakPtr = AsWeak();

	AsyncTask(ENamedThreads::GameThread, [WeakPtr, bConnected]
		{
			if (WeakPtr.IsValid())
			{
				TSharedPtr<FGNSession> ThisPtr = WeakPtr.Pin();
				if (ThisPtr->OnConnectResult.IsBound())
				{
					ThisPtr->OnConnectResult.Execute(bConnected);
				}
			}
		});

	if (!bConnected)
	{
		return 0;
	}

	Socket->SetNonBlocking(true);
	bIsRunning = true;

	while (bIsRunning)
	{
		bool bHasData = Socket->Wait(ESocketWaitConditions::WaitForRead, FTimespan::FromMilliseconds(10));

		if (bHasData)
		{
			RecvPacket();
		}

		SendPacket();

	}

	return 1;
}

void FGNSession::Stop()
{
	if (bIsRunning)
	{
		GN_LOG("Session Stop");
		bIsRunning = false;
	}
}

void FGNSession::Exit()
{
	GN_LOG("Session Exit");

	if (Socket)
	{
		Socket->Close();
		Socket = nullptr;
	}
}

bool FGNSession::TryConnect()
{
	Socket = FTcpSocketBuilder(TEXT("Server Session"))
		.AsReusable()
		.BoundToPort(0)
		.AsBlocking()
		.Build();

	if (!Socket)
	{
		GN_ERR("Socket Create Failed");
		return false;
	}

	Socket->SetNoDelay(true);

	TSharedRef<FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool bIsValid;
	Addr->SetIp(*IP, bIsValid);
	Addr->SetPort(Port);

	bool bConnected = Socket->Connect(*Addr);
	if (!bConnected)
	{
		return false;
	}

	return true;
}

void FGNSession::Disconnect()
{
	Stop();
	return;
}

void FGNSession::RegisterSend(TSharedPtr<FSendBuffer> SendBuffer)
{
	SendQueue.Enqueue(SendBuffer);
}

int32 FGNSession::RecvPacket()
{
	int32 ByteRead;
	bool bSuccess = Socket->Recv(RecvBuffer->WritePos(), RecvBuffer->FreeSize(), ByteRead);
	if (!bSuccess || ByteRead <= 0 || !RecvBuffer->OnWrite(ByteRead))
	{
		Disconnect();
		return 0;
	}

	int ProcessLen = HandlePacket();
	if (ProcessLen < 0 || RecvBuffer->DataSize() < ProcessLen || !RecvBuffer->OnRead(ProcessLen))
	{
		Disconnect();
		return 0;
	}

	RecvBuffer->Clean();

	return ProcessLen;
}

int32 FGNSession::HandlePacket()
{
	int32 ProcessLen = 0;
	int32 TotalSize = RecvBuffer->DataSize();
	BYTE* Buffer = RecvBuffer->ReadPos();
	int32 PacketCnt = 0;

	TSharedPtr<FGNSession> Session = SharedThis(this);
	while (true)
	{
		int32 DataSize = TotalSize - ProcessLen;

		if (DataSize < sizeof(PacketHeader)) break;

		PacketHeader* Header = reinterpret_cast<PacketHeader*>(Buffer + ProcessLen);
		if (DataSize < Header->size) break;

		if (Header->size <= 0 || Header->size > 4096)
		{
			GN_ERR("Invalid Packet Size: %d", Header->size);
			return -1;
		}

		OnRecvPacket.ExecuteIfBound(Session, Buffer + ProcessLen, Header->size);

		ProcessLen += Header->size;
		PacketCnt++;
	}

	GN_LOG("Recv (%d) Packet, Size : %d", PacketCnt, ProcessLen);

	return ProcessLen;
}

int32 FGNSession::SendPacket()
{
	if (SendQueue.IsEmpty()) return 0;

	int32 TotalSize = 0;
	int32 PacketCnt = 0;

	SendBufferChunk.Reset();

	while (!SendQueue.IsEmpty())
	{
		TSharedPtr<FSendBuffer> SendBuffer = nullptr;

		if (!SendQueue.Peek(SendBuffer)) break;
		if (MaxSendSize < TotalSize + SendBuffer->WriteSize()) break;

		SendQueue.Pop();
		TotalSize += SendBuffer->WriteSize();
		PacketCnt++;
		SendBufferChunk.Append(SendBuffer->Buffer(), SendBuffer->WriteSize());
	}

	int32 RemainSize = TotalSize;
	BYTE* BufferPtr = SendBufferChunk.GetData();
	while (RemainSize > 0)
	{
		int32 ByteSent;
		bool bSuccess = Socket->Send(BufferPtr, RemainSize, ByteSent);
		if (!bSuccess || ByteSent <= 0)
		{
			GN_WARN("Send Failed", PacketCnt, TotalSize);

			Disconnect();
			return -1;
		}

		BufferPtr += ByteSent;
		RemainSize -= ByteSent;
	}
	GN_LOG("Send (%d) Packet, Size : %d", PacketCnt, TotalSize);

	return TotalSize;
}
