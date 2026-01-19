// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class GAMENET_API FSendBuffer : public TSharedFromThis<FSendBuffer>
{
public:
	FSendBuffer(int32 BufferSize);
	~FSendBuffer();

	BYTE* Buffer() { return _Buffer.GetData(); }
	int32 WriteSize() { return _WriteSize; }
	int32 Capacity() { return static_cast<int32>(_Buffer.Max()); }

	void CopyData(void* data, int32 len);
	void Close(uint32 Size);



private:
	TArray<BYTE>	_Buffer;
	uint32			_WriteSize = 0;
};
