//===========
// Email.cpp
//===========

#include "pch.h"


//=======
// Using
//=======

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <mbedtls/base64.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include "Core/Application.h"
#include "Email.h"

using namespace Core;


//===========
// Namespace
//===========

namespace Network {
	namespace Smtp {


//==================
// Con-/Destructors
//==================

Email::Email(SmtpServer server):
pCertificate(server.Certificate),
pPort(server.Port),
pServer(server.Name)
{}


//========
// Common
//========

VOID Email::Send()
{
if(!User||!Password||!From||!To||!Subject||!Text)
	{
	DebugPrint("Email - Missing Parameter\n");
	return;
	}
hCallback=this;
xTaskCreate(SendProc, "email_task", 12288, this, 5, nullptr);
}


//================
// Common Private
//================

VOID Email::DoAbort(VOID* pssl, VOID* pnet)
{
if(pssl)
	mbedtls_ssl_session_reset((mbedtls_ssl_context*)pssl);
if(pnet)
	mbedtls_net_free((mbedtls_net_context*)pnet);
Application::Current->Dispatch(this, &Email::OnAborted);
}

VOID Email::DoSend()
{
mbedtls_ssl_context ssl;
mbedtls_ssl_init(&ssl);
mbedtls_ctr_drbg_context random;
mbedtls_ctr_drbg_init(&random);
mbedtls_entropy_context entropy;
mbedtls_entropy_init(&entropy);
if(mbedtls_ctr_drbg_seed(&random, mbedtls_entropy_func, &entropy, NULL, 0)!=0)
	{
	DebugPrint("Email: mbedtls_ctr_drbg_seed()\n");
	DoAbort(nullptr, nullptr);
	return;
	}
mbedtls_x509_crt cert;
mbedtls_x509_crt_init(&cert);
UINT cert_len=StringLength(pCertificate)+1;
if(mbedtls_x509_crt_parse(&cert, (BYTE const*)pCertificate, cert_len)!=0)
	{
	DebugPrint("Email: mbedtls_x509_crt_parse()\n");
	DoAbort(nullptr, nullptr);
	return;
	}
if(mbedtls_ssl_set_hostname(&ssl, pServer)!=0)
	{
	DebugPrint("Email: mbedtls_ssl_set_hostname()\n");
	DoAbort(nullptr, nullptr);
	return;
	}
mbedtls_ssl_config sslcfg;
mbedtls_ssl_config_init(&sslcfg);
if(mbedtls_ssl_config_defaults(&sslcfg, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)!=0)
	{
	DebugPrint("Email: mbedtls_ssl_config_defaults()\n");
	DoAbort(nullptr, nullptr);
	return;
	}
mbedtls_ssl_conf_authmode(&sslcfg, MBEDTLS_SSL_VERIFY_REQUIRED);
mbedtls_ssl_conf_ca_chain(&sslcfg, &cert, NULL);
mbedtls_ssl_conf_rng(&sslcfg, mbedtls_ctr_drbg_random, &random);
if(mbedtls_ssl_setup(&ssl, &sslcfg)!=0)
	{
	DebugPrint("Email: mbedtls_ssl_setup()\n");
	DoAbort(nullptr, nullptr);
	return;
	}
mbedtls_net_context net;
mbedtls_net_init(&net);
if(mbedtls_net_connect(&net, pServer, pPort, MBEDTLS_NET_PROTO_TCP)!=0)
	{
	DebugPrint("Email: mbedtls_net_connect()\n");
	DoAbort(&ssl, nullptr);
	return;
	}
mbedtls_ssl_set_bio(&ssl, &net, mbedtls_net_send, mbedtls_net_recv, NULL);
CHAR pbuf[512];
INT istatus=0;
size_t ulen=ReceiveData(&net, pbuf, 512, &istatus);
if(!ulen||istatus<200||istatus>299)
	{
	DebugPrint("Email: mbedtls_ssl_set_bio()\n");
	DoAbort(&ssl, &net);
	return;
	}
ulen=StringPrint(pbuf, 512, "EHLO esp\r\n");
ulen=SendAndReceive(&net, pbuf, ulen, 512, &istatus);
if(!ulen||istatus<200||istatus>299)
	{
	DebugPrint("Email: EHLO esp\n");
	DoAbort(&ssl, &net);
	return;
	}
ulen=StringPrint(pbuf, 512, "STARTTLS\r\n");
ulen=SendAndReceive(&net, pbuf, ulen, 512, &istatus);
if(!ulen||istatus<200||istatus>299)
	{
	DebugPrint("Email: STARTTLS\n");
	DoAbort(&ssl, &net);
	return;
	}
while((istatus=mbedtls_ssl_handshake(&ssl))!=0)
	{
	if(istatus!=MBEDTLS_ERR_SSL_WANT_READ&&istatus!=MBEDTLS_ERR_SSL_WANT_WRITE)
		{
		UINT res=mbedtls_ssl_get_verify_result(&ssl);
		DebugPrint("Email: mbedtls_ssl_handshake(): %u\n", res);
		DoAbort(&ssl, &net);
		return;
		}
	}
if(mbedtls_ssl_get_verify_result(&ssl)!=0)
	{
	DebugPrint("Email: mbedtls_ssl_get_verify_result()\n");
	DoAbort(&ssl, &net);
	return;
	}
ulen=StringPrint(pbuf, 512, "EHLO esp\r\n");
ulen=SendAndReceiveSsl(&ssl, pbuf, ulen, 512, &istatus);
if(!ulen||istatus<200||istatus>299)
	{
	DebugPrint("Email: EHLO esp\n");
	DoAbort(&ssl, &net);
	return;
	}
ulen=StringPrint(pbuf, 512, "AUTH LOGIN\r\n");
ulen=SendAndReceiveSsl(&ssl, pbuf, ulen, 512, &istatus);
if(!ulen||istatus<200||istatus>399)
	{
	DebugPrint("Email: AUTH LOGIN\n");
	DoAbort(&ssl, &net);
	return;
	}
istatus=mbedtls_base64_encode((BYTE*)pbuf, 128, &ulen, (BYTE const*)User->Begin(), User->GetLength());
if(istatus!=0)
	{
	DebugPrint("Email: mbedtls_base64_encode() USER\n");
	DoAbort(&ssl, &net);
	return;
	}
pbuf[ulen++]='\r';
pbuf[ulen++]='\n';
pbuf[ulen]=0;
ulen=SendAndReceiveSsl(&ssl, pbuf, ulen, 512, &istatus);
if(!ulen||istatus<300||istatus>399)
	{
	DebugPrint("Email: USER\n");
	DoAbort(&ssl, &net);
	return;
	}
istatus=mbedtls_base64_encode((BYTE*)pbuf, 128, &ulen, (BYTE const*)Password->Begin(), Password->GetLength());
if(istatus!=0)
	{
	DebugPrint("Email: mbedtls_base64_encode() PASSWD\n");
	DoAbort(&ssl, &net);
	return;
	}
pbuf[ulen++]='\r';
pbuf[ulen++]='\n';
pbuf[ulen]=0;
ulen=SendAndReceiveSsl(&ssl, pbuf, ulen, 512, &istatus);
if(!ulen||istatus<200||istatus>399)
	{
	DebugPrint("Email: PASSWD\n");
	DoAbort(&ssl, &net);
	return;
	}
auto pfrom=From->Begin();
ulen=StringPrint(pbuf, 512, "MAIL FROM:<%s>\r\n", pfrom);
ulen=SendAndReceiveSsl(&ssl, pbuf, ulen, 512, &istatus);
if(!ulen||istatus<200||istatus>299)
	{
	DebugPrint("Email: MAIL FROM\n");
	DoAbort(&ssl, &net);
	return;
	}
auto pto=To->Begin();
ulen=StringPrint(pbuf, 512, "RCPT TO:<%s>\r\n", pto);
ulen=SendAndReceiveSsl(&ssl, pbuf, ulen, 512, &istatus);
if(!ulen||istatus<200||istatus>299)
	{
	DebugPrint("Email: RCPT TO\n");
	DoAbort(&ssl, &net);
	return;
	}
ulen=StringPrint(pbuf, 512, "DATA\r\n");
ulen=SendAndReceiveSsl(&ssl, pbuf, ulen, 512, &istatus);
if(!ulen||istatus<300||istatus>399)
	{
	DebugPrint("Email: DATA\n");
	DoAbort(&ssl, &net);
	return;
	}
auto psubject=Subject->Begin();
ulen=StringPrint(pbuf, 512, "From: %s\r\nSubject: %s\r\nTo: %s\r\n\r\n", pfrom, psubject, pto);
ulen=SendDataSsl(&ssl, pbuf, ulen);
if(!ulen)
	{
	DebugPrint("Email: HEAD\n");
	DoAbort(&ssl, &net);
	return;
	}
auto ptext=Text->Begin();
ulen=Text->GetLength();
ulen=SendDataSsl(&ssl, ptext, ulen);
if(!ulen)
	{
	DebugPrint("Email: MSG\n");
	DoAbort(&ssl, &net);
	return;
	}
ulen=StringPrint(pbuf, 512, "\r\n.\r\n");
ulen=SendAndReceiveSsl(&ssl, pbuf, ulen, 512, &istatus);
if(!ulen||istatus<200||istatus>299)
	{
	DebugPrint("Email: FOOT\n");
	DoAbort(&ssl, &net);
	return;
	}
mbedtls_ssl_close_notify(&ssl);
mbedtls_ssl_session_reset(&ssl);
mbedtls_net_free(&net);
Application::Current->Dispatch(this, &Email::OnSent);
}

VOID Email::OnAborted()
{
Aborted(this);
hCallback=nullptr;
}

VOID Email::OnSent()
{
Sent(this);
hCallback=nullptr;
}

SIZE_T Email::ReceiveData(VOID* pnet, LPSTR pbuf, SIZE_T usize, INT* pstatus)
{
*pstatus=-1;
INT istatus=mbedtls_net_recv((mbedtls_net_context*)pnet, (BYTE*)pbuf, usize);
if(istatus<=0)
	{
	DebugPrint("SmtpClient - mbedtls_net_recv() failed (%i)\n", istatus);
	return 0;
	}
pbuf[istatus]=0;
StringScan(pbuf, "%i", pstatus);
return (SIZE_T)istatus;
}

SIZE_T Email::ReceiveDataSsl(VOID* pssl, LPSTR pbuf, SIZE_T usize, INT* pstatus)
{
*pstatus=-1;
INT istatus=0;
while((istatus=mbedtls_ssl_read((mbedtls_ssl_context*)pssl, (BYTE*)pbuf, usize))<=0)
	{
	if(istatus!=MBEDTLS_ERR_SSL_WANT_READ&&istatus!=MBEDTLS_ERR_SSL_WANT_WRITE)
		{
		DebugPrint("mbedtls_ssl_read() failed (%i)\n", istatus);
		return 0;
		}
	}
pbuf[istatus]=0;
StringScan(pbuf, "%i", pstatus);
return (SIZE_T)istatus;
}

SIZE_T Email::SendAndReceive(VOID* pnet, LPSTR pbuf, SIZE_T usize, SIZE_T ubufsize, INT* pstatus)
{
*pstatus=-1;
if(!SendData(pnet, pbuf, usize))
	return 0;
return ReceiveData(pnet, pbuf, ubufsize, pstatus);
}

SIZE_T Email::SendAndReceiveSsl(VOID* pssl, LPSTR pbuf, SIZE_T usize, SIZE_T ubufsize, INT* pstatus)
{
*pstatus=-1;
if(!SendDataSsl(pssl, pbuf, usize))
	return 0;
return ReceiveDataSsl(pssl, pbuf, ubufsize, pstatus);
}

SIZE_T Email::SendData(VOID* pnet, LPCSTR pbuf, SIZE_T usize)
{
INT istatus=mbedtls_net_send((mbedtls_net_context*)pnet, (BYTE const*)pbuf, usize);
if(istatus<=0)
	return 0;
return (SIZE_T)istatus;
}

SIZE_T Email::SendDataSsl(VOID* pssl, LPCSTR pbuf, SIZE_T usize)
{
INT istatus=0;
while((istatus=mbedtls_ssl_write((mbedtls_ssl_context*)pssl, (BYTE const*)pbuf, usize))<=0)
	{
	if(istatus!= MBEDTLS_ERR_SSL_WANT_READ&&istatus!= MBEDTLS_ERR_SSL_WANT_WRITE)
		return 0;
	}
return (SIZE_T)istatus;
}

VOID Email::SendProc(VOID* pparam)
{
Handle<Email> hemail=(Email*)pparam;
hemail->DoSend();
vTaskDelete(NULL);
}

}}