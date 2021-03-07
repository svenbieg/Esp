//===============
// DnsClient.cpp
//===============

#include "pch.h"


//=======
// Using
//=======

#include <lwip/dns.h>
#include "Core/Application.h"
#include "DnsClient.h"

using namespace Core;


//===========
// Namespace
//===========

namespace Network {
	namespace Dns {


//==================
// Con-/Destructors
//==================

DnsClient::DnsClient():
uIp(0)
{}

DnsClient::~DnsClient()
{
Application::Current->Loop.Remove(this);
}


//========
// Common
//========

VOID DnsClient::Lookup(Handle<String> hserver)
{
ip_addr_t ip;
err_t status=dns_gethostbyname(hserver->Begin(), &ip, (dns_found_callback)&DnsNameFoundProc, this);
if(status==ERR_INPROGRESS)
	{
	hCallback=this;
	return;
	}
if(status!=ERR_OK)
	{
	ServerNotFound(this);
	return;
	}
ServerFound(this, ip.IP4_UADDR);
}


//================
// Common Private
//================

VOID DnsClient::DnsNameFoundProc(LPCSTR pname, VOID const* paddr, VOID* parg)
{
Handle<DnsClient> hclient=(DnsClient*)parg;
hclient->hCallback=nullptr;
hclient->OnDnsNameFound(pname, paddr);
}

VOID DnsClient::OnDnsNameFound(LPCSTR pname, VOID const* paddr)
{
UINT uip=0;
ip_addr_t* pip=(ip_addr_t*)paddr;
if(pip)
	uip=pip->IP4_UADDR;
if(uip)
	{
	uIp=uip;
	Application::Current->Loop.AddProc(this, &DnsClient::OnServerFound);
	}
else
	{
	Application::Current->Loop.AddProc(this, &DnsClient::OnServerNotFound);
	}
}

VOID DnsClient::OnServerFound()
{
ServerFound(this, uIp);
}

VOID DnsClient::OnServerNotFound()
{
ServerNotFound(this);
}

}}