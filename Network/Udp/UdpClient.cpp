//===============
// UdpClient.cpp
//===============

#include "pch.h"


//=======
// Using
//=======

#include <lwip/udp.h>
#include "UdpClient.h"


//===========
// Namespace
//===========

namespace Network {
	namespace Udp {


//========
// Common
//========

VOID UdpClient::Broadcast(WORD uport, VOID const* pmsg, WORD usize)
{
udp_init();
udp_pcb* pcb=udp_new();
if(!pcb)
	{
	DebugPrint("UdpClient::Broadcast - udp_new() failed\n");
	return;
	}
pbuf* pb=pbuf_alloc(PBUF_IP, usize, PBUF_RAM);
if(!pb)
	{
	DebugPrint("UdpClient::Broadcast - pbuf_alloc() failed\n");
	return;
	}
CopyMemory(pb->payload, pmsg, usize);
ip_addr_t addr;
#ifdef ESP32
addr.type=IPADDR_TYPE_V4;
#endif
addr.IP4_UADDR=0xFFFFFFFF;
if(udp_sendto(pcb, pb, &addr, uport)!=ESP_OK)
	{
	DebugPrint("UdpClient::Broadcast - udp_sendto() failed\n");
	}
pbuf_free(pb);
}

}}