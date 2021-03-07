//===================
// TcpConnection.cpp
//===================

#include "pch.h"


//=======
// Using
//=======

#include <lwip/tcp.h>
#include <lwip/tcpip.h>
#include "TcpConnection.h"


//===========
// Namespace
//===========

namespace Network {
	namespace Tcp {


//==================
// Con-/Destructors
//==================

TcpConnection::TcpConnection(VOID* pcontext):
pContext(pcontext),
uStatus(TcpConnectionStatus::Open),
uTimeout(0)
{
InputBuffer=new PacketBuffer();
InitCallbacks();
SetTimeout(1000);
}

TcpConnection::~TcpConnection()
{
CloseInternal();
}


//========
// Common
//========

WORD TcpConnection::AvailableForWrite()
{
tcp_pcb* pcb=(tcp_pcb*)pContext;
return pcb->snd_buf;
}

VOID TcpConnection::Close()
{
if(uStatus==TcpConnectionStatus::Closed)
	return;
CloseInternal();
Closed(this);
}

Handle<HostAddress> TcpConnection::GetLocalAddress()
{
if(!pContext)
	return nullptr;
tcp_pcb* pcb=(tcp_pcb*)pContext;
Handle<IpAddress> hip=new IpAddress(pcb->local_ip.IP4_UADDR);
return new HostAddress(hip, pcb->local_port);
}

Handle<HostAddress> TcpConnection::GetRemoteAddress()
{
if(!pContext)
	return nullptr;
tcp_pcb* pcb=(tcp_pcb*)pContext;
Handle<IpAddress> hip=new IpAddress(pcb->remote_ip.IP4_UADDR);
return new HostAddress(hip, pcb->remote_port);
}

VOID TcpConnection::KeepAlive()
{
if(!pContext)
	return;
tcp_pcb* pcb=(tcp_pcb*)pContext;
SetFlag(pcb->so_options, SOF_KEEPALIVE);
pcb->keep_idle=7200000;
pcb->keep_intvl=75000;
pcb->keep_cnt=9;
}

VOID TcpConnection::SetTimeout(UINT utimeout)
{
UINT64 utime=GetTickCount64();
uTimeout=utime+utimeout;
}

VOID TcpConnection::Skip()
{
InputBuffer->Clear();
}


//==============
// Input-Stream
//==============

SIZE_T TcpConnection::Available()
{
return InputBuffer->Available();
}

SIZE_T TcpConnection::Read(VOID* pbuf, SIZE_T usize)
{
WORD uread=(WORD)MIN(usize, 0xFFFF);
return InputBuffer->Read(pbuf, uread);
}


//===============
// Output-Stream
//===============

VOID TcpConnection::Flush()
{
tcp_pcb* pcb=(tcp_pcb*)pContext;
tcp_output(pcb);
}

SIZE_T TcpConnection::Write(VOID const* pbuf, SIZE_T usize)
{
tcp_pcb* pcb=(tcp_pcb*)pContext;
WORD uavailable=pcb->snd_buf;
WORD uwrite=(WORD)MIN(uavailable, usize);
err_t status=tcp_write(pcb, pbuf, uwrite, TCP_WRITE_FLAG_COPY|TCP_WRITE_FLAG_MORE);
if(status!=ERR_OK)
	{
	DebugPrint("tcp_write failed (%i)\n", (INT)status);
	Close();
	return 0;
	}
SetTimeout(1000);
return uwrite;
}


//===================
// Callbacks Private
//===================

VOID TcpConnection::ErrorProc(VOID* parg, ERR_T err)
{
Handle<TcpConnection> hcon=(TcpConnection*)parg;
hcon->OnError(err);
}

VOID TcpConnection::InitCallbacks()
{
tcp_pcb* pcb=(tcp_pcb*)pContext;
tcp_setprio(pcb, TCP_PRIO_MIN);
tcp_arg(pcb, this);
tcp_err(pcb, (tcp_err_fn)&ErrorProc);
tcp_poll(pcb, (tcp_poll_fn)&PollProc, 1);
tcp_recv(pcb, (tcp_recv_fn)&ReceivedProc);
tcp_sent(pcb, (tcp_sent_fn)&SentProc);
}

ERR_T TcpConnection::PollProc(VOID* parg, VOID* pcb)
{
Handle<TcpConnection> hcon=(TcpConnection*)parg;
return hcon->OnPoll();
}

ERR_T TcpConnection::ReceivedProc(VOID* parg, VOID* pcb, VOID* prxbuf, ERR_T err)
{
Handle<TcpConnection> hcon=(TcpConnection*)parg;
return hcon->OnReceivced(pcb, prxbuf, err);
}

VOID TcpConnection::RemoveCallbacks()
{
tcp_pcb* pcb=(tcp_pcb*)pContext;
tcp_err(pcb, nullptr);
tcp_poll(pcb, nullptr, 0);
tcp_recv(pcb, nullptr);
tcp_sent(pcb, nullptr);
}

ERR_T TcpConnection::SentProc(VOID* parg, VOID* pcb, WORD ulen)
{
Handle<TcpConnection> hcon=(TcpConnection*)parg;
return hcon->OnSent(pcb, ulen);
}

VOID TcpConnection::OnError(ERR_T err)
{
if(err==ERR_ABRT)
	pContext=nullptr;
Close();
}

ERR_T TcpConnection::OnPoll()
{
if(uTimeout)
	{
	if(GetTickCount64()>uTimeout)
		{
		Close();
		return ERR_OK;
		}
	}
Send(this);
return ERR_OK;
}

ERR_T TcpConnection::OnReceivced(VOID* pcbv, VOID* prxbuf, ERR_T err)
{
if(prxbuf==nullptr)
	{
	DataSent(this);
	return ERR_OK;
	}
tcp_pcb* pcb=(tcp_pcb*)pContext;
pbuf* p=(pbuf*)prxbuf;
InputBuffer->Append(p);
tcp_recved(pcb, p->tot_len);
DataReceived(this);
return ERR_OK;
}

ERR_T TcpConnection::OnSent(VOID* pcb, WORD usize)
{
Send(this);
return ERR_OK;
}


//================
// Common Private
//================

VOID TcpConnection::CloseInternal()
{
if(pContext)
	{
	RemoveCallbacks();
	if(tcp_close((tcp_pcb*)pContext)!=ERR_OK)
		tcp_abort((tcp_pcb*)pContext);
	pContext=nullptr;
	}
InputBuffer->Clear();
uStatus=TcpConnectionStatus::Closed;
uTimeout=0;
}

}}