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
pSemaphore=xSemaphoreCreateBinary();
ASSERT(pSemaphore);
xSemaphoreGive(pSemaphore);
}

CriticalSection::~CriticalSection()
{
if(pSemaphore)
	vSemaphoreDelete(pSemaphore);
}


//========
// Common
//========

BOOL CriticalSection::IsLocked()const
{
if(!pSemaphore)
	return false;
return uxSemaphoreGetCount(pSemaphore)==0;
}

VOID CriticalSection::Lock()
{
if(xSemaphoreTake(pSemaphore, portMAX_DELAY)==pdFALSE)
	{
	DebugPrint("CriticalSection::Lock() - DeadLock\n");
	abort();
	}
}

BOOL CriticalSection::TryLock()
{
if(xSemaphoreTake(pSemaphore, 0)==pdTRUE)
	return true;
return false;
}

VOID CriticalSection::Unlock()
{
xSemaphoreGive(pSemaphore);
}
