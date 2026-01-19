// Fill out your copyright notice in the Description page of Project Settings.


#include "RecvBuffer.h"

FRecvBuffer::FRecvBuffer(int32 BufferSize) : _bufferSize(BufferSize)
{
	_capacity = BufferSize * BUFFER_COUNT;
	_buffer.SetNumUninitialized(_capacity);
}

FRecvBuffer::~FRecvBuffer()
{
}

void FRecvBuffer::Clean()
{
	int32 dataSize = DataSize();
	if (dataSize == 0)
	{
		// 딱 마침 읽기+쓰기 커서가 동일한 위치라면, 둘 다 리셋.
		_readPos = _writePos = 0;
	}
	else
	{
		// 여유 공간이 버퍼 1개 크기 미만이면, 데이터를 앞으로 땅긴다.
		if (FreeSize() < _bufferSize)
		{
			FMemory::Memcpy(&_buffer[0], &_buffer[_readPos], dataSize);
			_readPos = 0;
			_writePos = dataSize;
		}
	}
}

bool FRecvBuffer::OnRead(int32 numOfBytes)
{
	if (numOfBytes > DataSize())
		return false;

	_readPos += numOfBytes;
	return true;
}

bool FRecvBuffer::OnWrite(int32 numOfBytes)
{
	if (numOfBytes > FreeSize())
		return false;

	_writePos += numOfBytes;
	return true;
}
