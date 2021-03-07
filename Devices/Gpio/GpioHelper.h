//==============
// GpioHelper.h
//==============

#pragma once


//===========
// Namespace
//===========

namespace Devices {
	namespace Gpio {


//========
// Common
//========

enum class PinMode
{
Input=1,
Output=2,
InputPullup=3
};

BOOL DigitalRead(BYTE Pin);
VOID DigitalWrite(BYTE Pin, BOOL Set);
VOID SetPinMode(BYTE Pin, PinMode Mode);


//============
// Interrupts
//============

VOID AttachInterrupt(BYTE Pin, VOID (*Procedure)(VOID*), VOID* Argument);
VOID DetachInterrupt(BYTE Pin);

}}
