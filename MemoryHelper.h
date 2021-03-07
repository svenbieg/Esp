//================
// MemoryHelper.h
//================

#pragma once


//=======
// Using
//=======

#include "TypeHelper.h"


//==========
// Settings
//==========

#define MAX_PATH 256
#define PAGE_SIZE 4096


//============
// Allocation
//============

VOID* Alloc(SIZE_T Size);
VOID* AlignedAlloc(SIZE_T Size, SIZE_T Align);
VOID AlignedFree(VOID* Buffer);
VOID* DeviceAlloc(SIZE_T Size);
VOID DeviceFree(VOID* Buffer);
VOID Free(VOID* Buffer);
