//=====================
// CriticalSection.cpp
//=====================

#include "pch.h"


//=======
// Using
//=======

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "CriticalSection.h"


//==================
// Con-/Destructors
//==================

CriticalSection::CriticalSection():
pSemaphore(nullptr)
{
pSemaphore=(VOID*)xSemaphoreCreateBinary();
ASSERT(pSemaphore);
xSemaphoreGive(pSemaphore);
}

CriticalSection::~CriticalSection()
{
if(pSemaphore)
	vSemaphoreDelete((QueueHandle_t)pSemaphore);
}


//========
// Common
//========

BOOL CriticalSection::IsLocked()const
{
if(!pSemaphore)
	return false;
return uxSemaphoreGetCount((QueueHandle_t)pSemaphore)==0;
}

VOID CriticalSection::Lock()
{
if(xSemaphoreTake((QueueHandle_t)pSemaphore, portMAX_DELAY)==pdFALSE)
	{
	DebugPrint("CriticalSection::Lock() failed\n");
	abort();
	}
}

BOOL CriticalSection::TryLock()
{
if(xSemaphoreTake((QueueHandle_t)pSemaphore, 0)==pdTRUE)
	return true;
return false;
}

VOID CriticalSection::Unlock()
{
xSemaphoreGive((QueueHandle_t)pSemaphore);
}
