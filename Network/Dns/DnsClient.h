//=============
// DnsClient.h
//=============

#pragma once


//===========
// Namespace
//===========

namespace Network {
	namespace Dns {


//============
// Dns-Client
//============

class DnsClient: public Object
{
public:
	// Con-/Destructors
	DnsClient();
	~DnsClient();

	// Common
	VOID Lookup(Handle<String> Server);
	Event<DnsClient, UINT> ServerFound;
	Event<DnsClient> ServerNotFound;

private:
	// Common
	static VOID DnsNameFoundProc(LPCSTR Name, VOID const* Address, VOID* Arg);
	VOID OnDnsNameFound(LPCSTR Name, VOID const* Address);
	VOID OnServerFound();
	VOID OnServerNotFound();
	Handle<DnsClient> hCallback;
	UINT uIp;
};

}}