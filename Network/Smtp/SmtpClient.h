//==============
// SmtpClient.h
//==============

#pragma once


//=======
// Using
//=======

#include "Network/Smtp/Email.h"


//===========
// Namespace
//===========

namespace Network {
	namespace Smtp {


//========
// Status
//========

enum class SmtpClientStatus
{
Closed,
Closing,
Sending
};


//=============
// SMTP-Client
//=============

class SmtpClient: public Object
{
public:
	// Con-/Destructors
	SmtpClient();
	~SmtpClient();

	// Common
	VOID Close();
	Event<SmtpClient> Aborted;
	VOID Send(Handle<Email> Email);
	Handle<String> Password;
	Event<SmtpClient> Sent;
	Handle<String> User;

private:
	// Common
	static VOID SendProc(VOID* Param);
	VOID DoAbort(VOID* SslContext, VOID* NetContext);
	VOID DoSend();
	VOID OnAborted();
	VOID OnSent();
	SIZE_T ReceiveData(VOID* NetContext, LPSTR Buffer, SIZE_T Size, INT* Status);
	SIZE_T ReceiveDataSsl(VOID* SslContext, LPSTR Buffer, SIZE_T Size, INT* Status);
	SIZE_T SendAndReceive(VOID* NetContext, LPSTR Buffer, SIZE_T Size, SIZE_T BufferSize, INT* Status);
	SIZE_T SendAndReceiveSsl(VOID* SslContext, LPSTR Buffer, SIZE_T Size, SIZE_T BufferSize, INT* Status);
	SIZE_T SendData(VOID* NetContext, LPCSTR Buffer, SIZE_T Size);
	SIZE_T SendDataSsl(VOID* SslContext, LPCSTR Buffer, SIZE_T Size);
	Handle<SmtpClient> hCallback;
	Handle<Email> hEmail;
	SmtpClientStatus uStatus;
};

}}