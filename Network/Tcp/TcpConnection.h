//=================
// TcpConnection.h
//=================

#pragma once


//=======
// Using
//=======

#include "Network/HostAddress.h"
#include "Storage/Streams/RandomAccessStream.h"
#include "PacketBuffer.h"


//===========
// Namespace
//===========

namespace Network {
	namespace Tcp {


//========
// Status
//========

enum class TcpConnectionStatus
{
Closed,
Closing,
Open,
Sending
};


//================
// Tcp-Connection
//================

class TcpConnection: public Storage::Streams::RandomAccessStream
{
private:
	// Using
	using HostAddress=Network::HostAddress;

public:
	// Con-/Destructors
	TcpConnection(VOID* Context);
	~TcpConnection();

	// Common
	WORD AvailableForWrite();
	VOID Close();
	Event<TcpConnection> Closed;
	Event<TcpConnection> DataReceived;
	Event<TcpConnection> DataSent;
	Handle<HostAddress> GetLocalAddress();
	Handle<HostAddress> GetRemoteAddress();
	Handle<PacketBuffer> InputBuffer;
	VOID KeepAlive();
	Event<TcpConnection> Send;
	VOID SetTimeout(UINT Timeout);
	VOID Skip();

	// Input-Stream
	SIZE_T Available()override;
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;

	// Output-Stream
	VOID Flush()override;
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

private:
	// Callbacks
	static VOID ErrorProc(VOID* Connection, ERR_T Error);
	VOID InitCallbacks();
	static ERR_T PollProc(VOID* Connection, VOID* Context);
	static ERR_T ReceivedProc(VOID* Connection, VOID* Context, VOID* Buffer, ERR_T Error);
	VOID RemoveCallbacks();
	static ERR_T SentProc(VOID* Connection, VOID* Context, WORD Length);
	VOID OnError(ERR_T Error);
	ERR_T OnPoll();
	ERR_T OnReceivced(VOID* Context, VOID* Buffer, ERR_T Error);
	ERR_T OnSent(VOID* Context, WORD Length);

	// Common
	VOID CloseInternal();
	VOID* pContext;
	TcpConnectionStatus uStatus;
	UINT64 uTimeout;
};

}}
