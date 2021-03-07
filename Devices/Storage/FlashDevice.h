//===============
// FlashDevice.h
//===============

#pragma once


//=======
// Using
//=======

#include "Devices/Storage/BlockDevice.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Storage {


//==============
// Flash-Device
//==============

class FlashDevice: public BlockDevice
{
public:
	// Con-/Destructors
	FlashDevice();

	// Common
	static Handle<FlashDevice> Current;
	VOID Erase(UINT64 Offset, UINT Size)override;
	VOID Flush()override;
	UINT GetBlockSize()override;
	UINT64 GetSize()override { return uSize; }
	SIZE_T Read(UINT64 Offset, VOID* Buffer, SIZE_T Size)override;
	BOOL SetSize(UINT64 Size)override;
	SIZE_T Write(UINT64 Offset, VOID const* Buffer, SIZE_T Size)override;

private:
	// Common
	UINT DiskOffset(UINT* Offset);
	UINT uOffset;
	UINT uSize;
};

}}
