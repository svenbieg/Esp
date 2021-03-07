//===============
// UdpServer.cpp
//===============

#include "pch.h"


//=======
// Using
//=======

#include <lwip/udp.h>
#include "UdpServer.h"

using namespace Runtime;


//===========
// Namespace
//===========

namespace Network {
	namespace Udp {


//===========
// Callbacks
//===========

VOID UdpReceivedCallback(VOID* parg, udp_pcb* pcb, pbuf* pb, ip_addr_t const* pip, WORD uport)
{
Handle<UdpServer> hserver=(UdpServer*)parg;
Handle<UdpMessage> hmsg=new UdpMessage((BYTE const*)pb->payload, pb->tot_len);
pbuf_free(pb);
hserver->MessageReceived(hserver, hmsg);
}


//==================
// Con-/Destructors
//==================

UdpServer::UdpServer():
pContext(nullptr),
uStatus(UdpServerStatus::Closed)
{}

UdpServer::~UdpServer()
{
Close();
}


//========
// Common
//========

VOID UdpServer::Close()
{
if(uStatus==UdpServerStatus::Closed)
	return;
if(pContext)
	{
	udp_pcb* pcb=(udp_pcb*)pContext;
	udp_remove(pcb);
	pContext=nullptr;
	}
uStatus=UdpServerStatus::Closed;
}

VOID UdpServer::Listen(WORD uport)
{
Close();
udp_init();
udp_pcb* pcb=udp_new();
if(!pcb)
	{
	DebugPrint("udp_new() failed\n");
	return;
	}
udp_recv(pcb, &UdpReceivedCallback, this);
if(udp_bind(pcb, IP_ADDR_ANY, uport)!=ESP_OK)
	{
	DebugPrint("udp_bind() failed\n");
	udp_remove(pcb);
	return;
	}
pContext=pcb;
uStatus=UdpServerStatus::Listening;
}

}}