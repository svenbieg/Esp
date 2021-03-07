//=================
// InterruptLock.h
//=================

#pragma once


//=======
// Using
//=======

#include "Devices/Cpu.h"


//===========
// Namespace
//===========

namespace Devices {


//================
// Interrupt-Lock
//================

class InterruptLock
{
public:
	// Con-/Destructors
	InterruptLock() { Cpu::DisableInterrupts(); }
	~InterruptLock() { Cpu::EnableInterrupts(); }
};

}
