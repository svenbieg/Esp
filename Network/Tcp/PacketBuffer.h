//================
// PacketBuffer.h
//================

#pragma once


//===========
// Namespace
//===========

namespace Network {
	namespace Tcp {


//===============
// Packet-Buffer
//===============

class PacketBuffer: public Object
{
public:
	// Con-/Destructors
	PacketBuffer();
	~PacketBuffer();

	// Access
	WORD Available();
	BOOL Contains(VOID const* Buffer, SIZE_T Size);
	WORD Read(VOID* Buffer, WORD Size);

	// Modification
	WORD Append(VOID* PacketBuffer);
	VOID Clear();

private:
	// Common
	WORD Consume(WORD Size);
	VOID* pBuffer;
	WORD uPosition;
};

}}
