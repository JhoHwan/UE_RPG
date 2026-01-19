// Fill out your copyright notice in the Description page of Project Settings.


#include "SendBuffer.h"

FSendBuffer::FSendBuffer(int32 BufferSize) 
{
	_Buffer.Reserve(BufferSize);
}

FSendBuffer::~FSendBuffer()
{
}

void FSendBuffer::CopyData(void* data, int32 len)
{
	check(Capacity() >= len);
	FMemory::Memcpy(Buffer(), data, len);
	_WriteSize = len;
}

void FSendBuffer::Close(uint32 Size)
{
	_WriteSize = Size;
}