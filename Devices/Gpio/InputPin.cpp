//==============
// InputPin.cpp
//==============

#include "pch.h"


//=======
// Using
//=======

#include <esp_attr.h>
#include "Devices/Gpio/GpioHelper.h"
#include "Runtime/Application.h"
#include "InputPin.h"

using namespace Runtime;


//===========
// Namespace
//===========

namespace Devices {
	namespace Gpio {


//============
// Interrupts
//============

UINT64 IRAM_ATTR uChangedPin=0;

VOID IRAM_ATTR InterruptProc(VOID* parg)
{
UINT upin=(UINT)parg;
uChangedPin|=(1ULL<<upin);
}


//==================
// Con-/Destructors
//==================

InputPin::InputPin(Handle<String> hid, BYTE upin):
uPin(upin)
{
SetPinMode(uPin, PinMode::Input);
BOOL bvalue=DigitalRead(uPin);
Value=new Bool(hid, bvalue);
AttachInterrupt(uPin, InterruptProc, (VOID*)(UINT)uPin);
Application::Current->Loop.Add(this, &InputPin::OnLoop);
}

InputPin::~InputPin()
{
DetachInterrupt(uPin);
Application::Current->Loop.Remove(this);
}


//================
// Common Private
//================

VOID InputPin::OnLoop()
{
UINT64 umask=1ULL<<uPin;
BOOL bchanged=(uChangedPin&umask)>0;
if(!bchanged)
	return;
uChangedPin&=~umask;
Value=DigitalRead(uPin);
Value? Up(this): Down(this);
Changed(this);
}

}}
