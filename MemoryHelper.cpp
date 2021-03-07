//==================
// MemoryHelper.cpp
//==================

#include "pch.h"


//=======
// Using
//=======

#include <esp_heap_caps.h>


//============
// Allocation
//============

VOID* Alloc(SIZE_T size)
{
return heap_caps_malloc(size, MALLOC_CAP_8BIT);
}

VOID* AlignedAlloc(SIZE_T size, SIZE_T ualign)
{
return heap_caps_malloc(size, MALLOC_CAP_32BIT);
}

VOID AlignedFree(VOID* pbuf)
{
heap_caps_free(pbuf);
}

VOID* DeviceAlloc(SIZE_T size)
{
return heap_caps_malloc(size, MALLOC_CAP_DMA);
}

VOID DeviceFree(VOID* pbuf)
{
heap_caps_free(pbuf);
}

VOID Free(VOID* pbuf)
{
heap_caps_free(pbuf);
}
