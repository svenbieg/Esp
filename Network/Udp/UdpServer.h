//=============
// UdpServer.h
//=============

#pragma once


//=======
// Using
//=======

#include "Network/Udp/UdpMessage.h"
#include "User/Storage/Buffer.h"


//===========
// Namespace
//===========

namespace Network {
	namespace Udp {


//========
// Status
//========

enum class UdpServerStatus
{
Closed,
Closing,
Listening
};


//============
// Udp-Server
//============

class UdpServer: public Object
{
private:
	// Using
	using Buffer=Storage::Buffer;

public:
	// Con-/Destructors
	UdpServer();
	~UdpServer();
	
	// Common
	VOID Close();
	VOID Listen(WORD Port);
	Event<UdpServer, Handle<UdpMessage>> MessageReceived;

private:
	// Common
	VOID* pContext;
	UdpServerStatus uStatus;
};

}}
