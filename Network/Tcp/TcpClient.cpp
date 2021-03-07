//===============
// TcpClient.cpp
//===============

#include "pch.h"


//=======
// Using
//=======

#include <lwip/err.h>
#include <lwip/tcp.h>
#include <lwip/tcpip.h>
#include "TcpClient.h"


//===========
// Namespace
//===========

namespace Network {
	namespace Tcp {


//==================
// Con-/Destructors
//==================

TcpClient::TcpClient():
pContext(nullptr),
uRemotePort(0),
uStatus(TcpClientStatus::Closed)
{}

TcpClient::~TcpClient()
{
CloseInternal();
}


//========
// Common
//========

VOID TcpClient::Close()
{
if(uStatus==TcpClientStatus::Closed)
	return;
CloseInternal();
Closed(this);
}

VOID TcpClient::Connect(UINT uip, WORD uport)
{
ASSERT(uip&&uport);
Close();
DoConnect(uip, uport);
}

VOID TcpClient::Connect(Handle<String> hurl, WORD uport)
{
ASSERT(hurl&&uport);
Close();
uRemotePort=uport;
uStatus=TcpClientStatus::DnsLookup;
hDnsClient=new DnsClient();
hDnsClient->ServerFound.Add(this, &TcpClient::OnDnsClientServerFound);
hDnsClient->ServerNotFound.Add(this, &TcpClient::OnDnsClientServerNotFound);
hDnsClient->Lookup(hurl);
}


//===================
// Callbacks Private
//===================

ERR_T TcpClient::ConnectedProc(VOID* parg, VOID* pcb, ERR_T status)
{
Handle<TcpClient> hclient=(TcpClient*)parg;
return hclient->OnConnectedCallback(pcb, status);
}

VOID TcpClient::ErrorProc(VOID* parg, ERR_T err)
{
Handle<TcpClient> hclient=(TcpClient*)parg;
hclient->OnErrorCallback(err);
}

ERR_T TcpClient::OnConnectedCallback(VOID* pcb, ERR_T status)
{
uStatus=TcpClientStatus::Connected;
Handle<TcpConnection> hcon=new TcpConnection(pcb);
pContext=nullptr;
Connected(this, hcon);
return ERR_OK;
}

VOID TcpClient::OnErrorCallback(ERR_T err)
{
if(err==ERR_ABRT)
	pContext=nullptr;
Close();
}


//================
// Common Private
//================

VOID TcpClient::CloseInternal()
{
if(pContext)
	{
	tcp_pcb* pcb=(tcp_pcb*)pContext;
	tcp_err(pcb, nullptr);
	if(tcp_close(pcb)!=ERR_OK)
		tcp_abort(pcb);
	pContext=nullptr;
	}
hDnsClient=nullptr;
uRemotePort=0;
uStatus=TcpClientStatus::Closed;
}

VOID TcpClient::DoConnect(UINT uip, WORD uport)
{
uStatus=TcpClientStatus::Connecting;
tcp_pcb* pcb=tcp_new();;
if(!pcb)
	{
	DebugPrint("TcpClient: tcp_new() failed\n");
	uStatus=TcpClientStatus::Closed;
	return;
	}
pContext=pcb;
/*pcb->so_options|=SOF_REUSEADDR;
if(tcp_bind(pcb, IP_ADDR_ANY, 0)!=ERR_OK)
	{
	DebugPrint("TcpClient - tcp_bind() failed\r\n");
	Close();
	return;
	}*/
ip_addr_t ip;
#ifdef ESP32
ip.type=IPADDR_TYPE_V4;
#endif
ip.IP4_UADDR=uip;
tcp_arg(pcb, this);
tcp_err(pcb, (tcp_err_fn)&ErrorProc);
err_t status=tcp_connect(pcb, &ip, uport, (tcp_connected_fn)&ConnectedProc);
if(status!=ERR_OK)
	{
	DebugPrint("TcpClient: tcp_connect() failed (%i)\n", (INT)status);
	Close();
	}
}

VOID TcpClient::OnDnsClientServerFound(Handle<DnsClient> hdns, UINT uip)
{
hDnsClient=nullptr;
DoConnect(uip, uRemotePort);
}

VOID TcpClient::OnDnsClientServerNotFound(Handle<DnsClient> hdns)
{
hDnsClient=nullptr;
uStatus=TcpClientStatus::Closed;
ServerNotFound(this);
}

}}