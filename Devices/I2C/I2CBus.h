//==========
// I2CBus.h
//==========

#pragma once


//=======
// Using
//=======

#include "Devices/I2C/I2CHelper.h"
#include "Storage/Streams/RandomAccessStream.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace I2C {


//=========
// I2C-Bus
//=========

class I2CBus: public ::Storage::Streams::RandomAccessStream
{
public:
	// Con-/Destructors
	I2CBus(BYTE SdaPin=DefaultSdaPin, BYTE SclPin=DefaultSclPin);
	~I2CBus();

	// Input-Stream
	SIZE_T Available()override;
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;

	// Ouput-Stream
	VOID Flush()override;
	SIZE_T Write(BYTE Value);
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

	// Common
	VOID BeginRead(BYTE Address);
	BOOL BeginWrite(BYTE Address);
	static Handle<I2CBus> Current;
	VOID EndTransmission();

private:
	// Common
	VOID* pCommand;
};

}}
