//=============
// TcpClient.h
//=============

#pragma once


//=======
// Using
//=======

#include "Network/Dns/DnsClient.h"
#include "TcpConnection.h"


//===========
// Namespace
//===========

namespace Network {
	namespace Tcp {


//========
// Status
//========

enum class TcpClientStatus
{
Closed,
DnsLookup,
Connecting,
Connected
};


//============
// Tcp-Client
//============

class TcpClient: public Object
{
public:
	// Using
	using DnsClient=Network::Dns::DnsClient;

	// Con-/Destructors
	TcpClient();
	~TcpClient();

	// Common
	VOID Close();
	Event<TcpClient> Closed;
	VOID Connect(UINT Ip, WORD Port);
	VOID Connect(Handle<String> Server, WORD Port);
	Event<TcpClient, Handle<TcpConnection>> Connected;
	Event<TcpClient> ServerNotFound;

private:
	// Callbacks
	static ERR_T ConnectedProc(VOID* Arg, VOID* Context, ERR_T Status);
	static VOID ErrorProc(VOID* Arg, ERR_T Error);
	ERR_T OnConnectedCallback(VOID* Context, ERR_T Status);
	VOID OnErrorCallback(ERR_T Error);

	// Common
	VOID CloseInternal();
	VOID DoConnect(UINT Ip, WORD Port);
	VOID OnDnsClientServerFound(Handle<DnsClient> Client, UINT Ip);
	VOID OnDnsClientServerNotFound(Handle<DnsClient> Client);
	Handle<DnsClient> hDnsClient;
	VOID* pContext;
	WORD uRemotePort;
	TcpClientStatus uStatus;
};

}}