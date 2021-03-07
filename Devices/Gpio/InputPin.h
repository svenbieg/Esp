//============
// InputPin.h
//============

#pragma once


//=======
// Using
//=======

#include "BoolClass.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Gpio {


//===========
// Input-Pin
//===========

class InputPin: public Object
{
public:
	// Con-/Destructors
	InputPin(Handle<String> Id, BYTE Pin);
	~InputPin();

	// Common
	Event<InputPin> Changed;
	Event<InputPin> Down;
	inline BYTE GetPin()const { return uPin; }
	Event<InputPin> Up;
	Handle<Bool> Value;

private:
	// Common
	VOID OnLoop();
	BYTE uPin;
};

}}
