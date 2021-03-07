//=============
// TcpServer.h
//=============

#pragma once


//=======
// Using
//=======

#include "TcpConnection.h"


//===========
// Namespace
//===========

namespace Network {
	namespace Tcp {


//========
// Status
//========

enum class TcpServerStatus
{
Closed,
Listening
};


//============
// Tcp-Server
//============

class TcpServer: public Object
{
public:
	// Con-/Destructors
	TcpServer();
	~TcpServer();

	// Common
	VOID Close();
	Event<TcpServer, Handle<TcpConnection>> ConnectionReceived;
	VOID Listen(WORD Port);

private:
	// Callbacks
	static ERR_T AcceptProc(VOID* Arg, VOID* Context, ERR_T Status);
	ERR_T OnAcceptCallback(VOID* Context, ERR_T Status);

	// Common
	VOID* pContext;
	TcpServerStatus uStatus;
};

}}
