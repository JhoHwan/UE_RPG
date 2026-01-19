// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class FRecvBuffer
{
	enum { BUFFER_COUNT = 10 };

public:
	FRecvBuffer(int32 BufferSize);
	~FRecvBuffer();

	void Clean();
	bool OnRead(int32 NumOfBytes);
	bool OnWrite(int32 NumOfBytes);

	BYTE* ReadPos() { return &_buffer[_readPos]; }
	BYTE* WritePos() { return &_buffer[_writePos]; }
	int32 DataSize() { return _writePos - _readPos; }
	int32 FreeSize() { return _capacity - _writePos; }

private:
	int32 _capacity = 0;
	int32 _bufferSize = 0;
	int32 _readPos = 0;
	int32 _writePos = 0;
	TArray<BYTE> _buffer;
};

