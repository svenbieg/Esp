//=============
// UdpClient.h
//=============

#pragma once


//===========
// Namespace
//===========

namespace Network {
	namespace Udp {


//============
// Udp-Client
//============

class UdpClient: public Object
{
public:
	// Common
	static VOID Broadcast(WORD Port, VOID const* Buffer, WORD Size);
};

}}