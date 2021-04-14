//=========
// Email.h
//=========

#pragma once


//=======
// Using
//=======

#include "Network/Smtp/SmtpServer.h"


//===========
// Namespace
//===========

namespace Network {
	namespace Smtp {


//========
// E-Mail
//========

class Email: public Object
{
public:
	// Con-/Destructors
	Email(SmtpServer Server=SmtpServerOutlook);

	// Common
	Event<Email> Aborted;
	Handle<String> From;
	Handle<String> Password;
	VOID Send();
	Event<Email> Sent;
	Handle<String> Subject;
	Handle<String> Text;
	Handle<String> To;
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
	Handle<Email> hCallback;
	LPCSTR pCertificate;
	LPCSTR pPort;
	LPCSTR pServer;
};

}}