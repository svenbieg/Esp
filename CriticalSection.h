//===================
// CriticalSection.h
//===================

#pragma once


//=======
// Using
//=======

#include "TypeHelper.h"


//==================
// Critical Section
//==================

class CriticalSection
{
public:
	// Con-/Destructors
	CriticalSection();
	CriticalSection(CriticalSection const& CriticalSection)=delete;
	~CriticalSection();

	// Common
	BOOL IsLocked()const;
	VOID Lock();
	BOOL TryLock();
	VOID Unlock();

private:
	// Common
	VOID* pSemaphore;
};
