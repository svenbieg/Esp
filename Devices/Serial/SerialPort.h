//==============
// SerialPort.h
//==============

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/RandomAccessStream.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Serial {


//===========
// Baud-Rate
//===========

enum class BaudRate
{
Baud57600=57600,
Baud74880=74880,
Baud115200=115200
};


//=============
// Serial Port
//=============

class SerialPort: public ::Storage::Streams::RandomAccessStream
{
private:
	// Using
	using OutputStream=::Storage::Streams::OutputStream;

public:
	// Con-/Destructors
	SerialPort(UINT Id, BaudRate BaudRate);
	~SerialPort();

	// Common
	static Handle<SerialPort> Current;

	// Input
	SIZE_T Available()override;
	SIZE_T Read(VOID* Buffer, SIZE_T Size);

	// Output
	SIZE_T AvailableForWrite();
	VOID Flush()override;
	VOID Print(LPCSTR Value);
	VOID Print(Handle<String> Value);
	VOID Print(UINT Length, LPCSTR String);
	template <class... _tParams> VOID Print(LPCSTR Format, _tParams... Params) { Print(new String(Format, Params...)); }
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

private:
	// Common
	UINT uId;
	UINT uTimeout;
};

}}
