//=============
// DnsServer.h
//=============

#pragma once


//===========
// Namespace
//===========

namespace Network {
	namespace Dns {


//===================
// Dns-Server Status
//===================

enum class DnsServerStatus
{
Closed,
Closing,
Listening
};


//============
// Dns-Server
//============

class DnsServer: public Object
{
public:
	// Con-/Destructors
	DnsServer();
	~DnsServer();
	Event<VOID, DnsServer*> Destroyed;

	// Common
	VOID Close();
	Handle<String> HostName;
	VOID Listen();

private:
	// Common
	static VOID DnsTask(VOID* Param);
	WORD HandleRequest(BYTE* Request, WORD Size, BYTE* Response);
	VOID DoListen();
	BYTE* ReadLabel(BYTE* Packet, WORD PacketSize, BYTE* Label, LPSTR String, WORD MaxLength);
	WORD ReadWord(VOID const* Buffer);
	BYTE* WriteLabel(BYTE* Packet, WORD PacketSize, BYTE* Label, LPCSTR String);
	VOID WriteWord(WORD* Buffer, WORD Value);
	Handle<DnsServer> hCallback;
	DnsServerStatus uStatus;
};

}}
