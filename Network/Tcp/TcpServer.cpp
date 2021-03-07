//===============
// TcpServer.cpp
//===============

#include "pch.h"


//=======
// Using
//=======

#include <lwip/tcp.h>
#include "TcpServer.h"


//===========
// Namespace
//===========

namespace Network {
	namespace Tcp {


//==================
// Con-/Destructors
//==================

TcpServer::TcpServer():
pContext(nullptr),
uStatus(TcpServerStatus::Closed)
{}

TcpServer::~TcpServer()
{
Close();
}


//========
// Common
//========

VOID TcpServer::Close()
{
if(uStatus==TcpServerStatus::Closed)
	return;
if(pContext)
	{
	tcp_pcb* pcb=(tcp_pcb*)pContext;
	if(tcp_close(pcb)!=ERR_OK)
		tcp_abort(pcb);
	pContext=nullptr;
	}
uStatus=TcpServerStatus::Closed;
}

VOID TcpServer::Listen(WORD uport)
{
Close();
tcp_pcb* pcb=tcp_new();
if(!pcb)
	{
	DebugPrint("TcpServer: tcp_new() failed\n");
	return;
	}
pContext=pcb;
uStatus=TcpServerStatus::Listening;
ip_addr_t addr;
#ifdef ESP32
addr.type=IPADDR_TYPE_V4;
#endif
addr.IP4_UADDR=IPADDR_ANY;
pcb->so_options|=SOF_REUSEADDR;
if(tcp_bind(pcb, &addr, uport)!=ERR_OK)
	{
	DebugPrint("TcpServer: tcp_bind() failed\n");
	Close();
	return;
	}
pContext=tcp_listen(pcb);
if(!pContext)
	{
	DebugPrint("TcpServer: tcp_listen() failed\n");
	Close();
	return;
	}
pcb=(tcp_pcb*)pContext;
tcp_arg(pcb, this);
tcp_accept(pcb, (tcp_accept_fn)&AcceptProc);
}


//===================
// Callbacks Private
//===================

ERR_T TcpServer::AcceptProc(VOID* parg, VOID* pcb, ERR_T status)
{
Handle<TcpServer> hserver=(TcpServer*)parg;
return hserver->OnAcceptCallback(pcb, status);
}

ERR_T TcpServer::OnAcceptCallback(VOID* pcb, ERR_T status)
{
Handle<TcpConnection> hcon=new TcpConnection(pcb);
ConnectionReceived(this, hcon);
return ERR_OK;
}

}}
