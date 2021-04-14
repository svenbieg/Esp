//==============
// SerialPort.h
//==============

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/RandomAccessStream.h"
#include "Devices/Serial/BaudRate.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Serial {


//=============
// Serial Port
//=============

class SerialPort: public ::Storage::Streams::RandomAccessStream
{
private:
	// Using
	using ListenTask=TaskTyped<SerialPort>;
	using OutputStream=::Storage::Streams::OutputStream;

public:
	// Con-/Destructors
	SerialPort(UINT Id, BaudRate BaudRate);
	~SerialPort();

	// Common
	VOID ClearBuffer();
	Event<SerialPort> DataReceived;
	VOID Listen();
	VOID StopListening();

	// Input
	SIZE_T Available()override;
	SIZE_T Read(VOID* Buffer, SIZE_T Size);

	// Output
	VOID Flush()override;
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

private:
	// Common
	VOID DoListen();
	CriticalSection cCriticalSection;
	Handle<ListenTask> hTask;
	VOID* pQueue;
	UINT uId;
};

}}
