//================
// SmtpClient.cpp
//================

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
#include "Storage/Buffer.h"
#include "SmtpClient.h"

using namespace Core;
using namespace Storage;


//===========
// Namespace
//===========

namespace Network {
	namespace Smtp {


//==========
// Settings
//==========

const CHAR pServer[]="smtp.web.de";
const CHAR pPort[]="587";

const BYTE pCertificate[]=
"-----BEGIN CERTIFICATE-----\r\n"
"MIIF1DCCBLygAwIBAgIIF00sptYyPA4wDQYJKoZIhvcNAQELBQAwgYIxCzAJBgNV\r\n"
"BAYTAkRFMSswKQYDVQQKDCJULVN5c3RlbXMgRW50ZXJwcmlzZSBTZXJ2aWNlcyBH\r\n"
"bWJIMR8wHQYDVQQLDBZULVN5c3RlbXMgVHJ1c3QgQ2VudGVyMSUwIwYDVQQDDBxU\r\n"
"LVRlbGVTZWMgR2xvYmFsUm9vdCBDbGFzcyAzMB4XDTE0MDIxMTE0NDQ1MFoXDTI0\r\n"
"MDIxMTIzNTk1OVowgfMxCzAJBgNVBAYTAkRFMSUwIwYDVQQKDBxULVN5c3RlbXMg\r\n"
"SW50ZXJuYXRpb25hbCBHbWJIMR8wHQYDVQQLDBZULVN5c3RlbXMgVHJ1c3QgQ2Vu\r\n"
"dGVyMRwwGgYDVQQIDBNOb3JkcmhlaW4gV2VzdGZhbGVuMQ4wDAYDVQQRDAU1NzI1\r\n"
"MDEQMA4GA1UEBwwHTmV0cGhlbjEgMB4GA1UECQwXVW50ZXJlIEluZHVzdHJpZXN0\r\n"
"ci4gMjAxOjA4BgNVBAMMMVRlbGVTZWMgU2VydmVyUGFzcyBFeHRlbmRlZCBWYWxp\r\n"
"ZGF0aW9uIENsYXNzIDMgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIB\r\n"
"AQDe9ITwcAUKw1Z1hbXzpITvs3w91uTP/2XnpuMkVZGimp7G5zSS9pWrvh3EiV1z\r\n"
"Xa2R9WOVetahRgn1nbpTaqCIl3kYQjaTiiRFW/g/YHoVSyYK1H5dhImjRc8terkT\r\n"
"hteHxNDNqwGK9bABfwgTvdEMGGjkWVLYZmr1SitQt8OD/TnPg8MXH050SeV7rxYW\r\n"
"4RxEjTPRaoDXtm9xjHNCqKxnznzuyWBIKYFNiDDF9wdBL+/7z23JZFynbsnURNoD\r\n"
"D+uZKGTikE6EauENr02vfm8fTcrQVHsVZ00u4aHWv4+Wowyjw/2nfBq/cIa03ate\r\n"
"BDpvJUJm0RWb1HzEJSmD1S9rAgMBAAGjggHZMIIB1TASBgNVHRMBAf8ECDAGAQH/\r\n"
"AgEAMEMGA1UdIAQ8MDowOAYEVR0gADAwMC4GCCsGAQUFBwIBFiJodHRwOi8vcGtp\r\n"
"LnRlbGVzZWMuZGUvY3BzL2Nwcy5odG1sMA4GA1UdDwEB/wQEAwIBBjCB7wYDVR0f\r\n"
"BIHnMIHkMDWgM6Axhi9odHRwOi8vcGtpLnRlbGVzZWMuZGUvcmwvR2xvYmFsUm9v\r\n"
"dF9DbGFzc18zLmNybDCBqqCBp6CBpIaBoWxkYXA6Ly9wa2kudGVsZXNlYy5kZS9D\r\n"
"Tj1ULVRlbGVTZWMlMjBHbG9iYWxSb290JTIwQ2xhc3MlMjAzLE9VPVQtU3lzdGVt\r\n"
"cyUyMFRydXN0JTIwQ2VudGVyLE89VC1TeXN0ZW1zJTIwRW50ZXJwcmlzZSUyMFNl\r\n"
"cnZpY2VzJTIwR21iSCxDPURFP0F1dGhvcml0eVJldm9jYXRpb25MaXN0MDgGCCsG\r\n"
"AQUFBwEBBCwwKjAoBggrBgEFBQcwAYYcaHR0cDovL29jc3AudGVsZXNlYy5kZS9v\r\n"
"Y3NwcjAdBgNVHQ4EFgQUMCzVT9II0T3jp4KSgjeJbWa5D68wHwYDVR0jBBgwFoAU\r\n"
"tQP3djthgmoSqhhT6wMhlL/+zsowDQYJKoZIhvcNAQELBQADggEBAHUN45xLBV0V\r\n"
"A6U2NGIfNS4wJAY86ShbTmQrOSTDr0dJP7hu3ADXzVw4JQtNlJ2akevmvb7T1xZ0\r\n"
"rE+ntKp2xMZpdPZxO8Rw2LO+3ds2enC9cDbEO1lHqpXkL9ybnu4yetO8hXetgqmL\r\n"
"GXE0LtLkprH3gc4Dmsm1EsVsOJlNDUmEwWbck/pMpJA9yLkO0s/VRd+KaL43HEOc\r\n"
"F88cPSiJJk1KzinC0yfutyR0HtRcycS5T68+XntH4ziLXCyCpdCI29bpTDY1rqrT\r\n"
"D+4T61tvJQ9qVcGk3h9nu0hZ1OlrYBBmTiwLF/BZy2td0MHf5Xmml3k6EJMAzH6B\r\n"
"u3BazPkHrzc=\r\n"
"-----END CERTIFICATE-----\r\n";


//==================
// Con-/Destructors
//==================

SmtpClient::SmtpClient():
uStatus(SmtpClientStatus::Closed)
{}

SmtpClient::~SmtpClient()
{
Close();
}


//========
// Common
//========

VOID SmtpClient::Close()
{
if(uStatus==SmtpClientStatus::Closed)
	return;
uStatus=SmtpClientStatus::Closing;
while(uStatus!=SmtpClientStatus::Closed)
	Sleep(10);
hEmail=nullptr;
Application::Current->Loop.Remove(this);
}

VOID SmtpClient::Send(Handle<Email> hemail)
{
Close();
if(!User||!Password)
	{
	DebugPrint("SmtpClient - No user and password\n");
	return;
	}
if(!hemail||!hemail->From||!hemail->To||!hemail->Subject||!hemail->Text)
	{
	DebugPrint("SmtpClient - No email or incomplete\n");
	return;
	}
hCallback=this;
hEmail=hemail;
uStatus=SmtpClientStatus::Sending;
xTaskCreate(SendProc, "SmtpTask", 8192, this, 5, nullptr);
}


//================
// Common Private
//================

VOID SmtpClient::DoAbort(VOID* pssl, VOID* pnet)
{
if(pssl)
	mbedtls_ssl_session_reset((mbedtls_ssl_context*)pssl);
if(pnet)
	mbedtls_net_free((mbedtls_net_context*)pnet);
if(uStatus!=SmtpClientStatus::Closing)
	Application::Current->Loop.AddProc(this, &SmtpClient::OnAborted);
}

VOID SmtpClient::DoSend()
{
mbedtls_ssl_context ssl;
mbedtls_ssl_init(&ssl);
mbedtls_ctr_drbg_context random;
mbedtls_ctr_drbg_init(&random);
mbedtls_entropy_context entropy;
mbedtls_entropy_init(&entropy);
if(mbedtls_ctr_drbg_seed(&random, mbedtls_entropy_func, &entropy, NULL, 0)!=0)
	{
	DoAbort(nullptr, nullptr);
	return;
	}
mbedtls_x509_crt cert;
mbedtls_x509_crt_init(&cert);
if(mbedtls_x509_crt_parse(&cert, pCertificate, ARRAYSIZE(pCertificate))!=0)
	{
	DoAbort(nullptr, nullptr);
	return;
	}
if(mbedtls_ssl_set_hostname(&ssl, pServer)!=0)
	{
	DoAbort(nullptr, nullptr);
	return;
	}
mbedtls_ssl_config sslcfg;
mbedtls_ssl_config_init(&sslcfg);
if(mbedtls_ssl_config_defaults(&sslcfg, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)!=0)
	{
	DoAbort(nullptr, nullptr);
	return;
	}
mbedtls_ssl_conf_authmode(&sslcfg, MBEDTLS_SSL_VERIFY_REQUIRED);
mbedtls_ssl_conf_ca_chain(&sslcfg, &cert, NULL);
mbedtls_ssl_conf_rng(&sslcfg, mbedtls_ctr_drbg_random, &random);
if(mbedtls_ssl_setup(&ssl, &sslcfg)!=0)
	{
	DoAbort(nullptr, nullptr);
	return;
	}
mbedtls_net_context net;
mbedtls_net_init(&net);
if(mbedtls_net_connect(&net, pServer, pPort, MBEDTLS_NET_PROTO_TCP)!=0)
	{
	DoAbort(&ssl, nullptr);
	return;
	}
mbedtls_ssl_set_bio(&ssl, &net, mbedtls_net_send, mbedtls_net_recv, NULL);
Handle<Buffer> hbuf=new Buffer(512);
auto pbuf=(LPSTR)hbuf->Begin();
INT istatus=0;
size_t ulen=ReceiveData(&net, pbuf, 512, &istatus);
if(!ulen||istatus<200||istatus>299)
	{
	DoAbort(&ssl, &net);
	return;
	}
ulen=StringPrint(pbuf, 512, "EHLO esp\r\n");
ulen=SendAndReceive(&net, pbuf, ulen, 512, &istatus);
if(!ulen||istatus<200||istatus>299)
	{
	DoAbort(&ssl, &net);
	return;
	}
ulen=StringPrint(pbuf, 512, "STARTTLS\r\n");
ulen=SendAndReceive(&net, pbuf, ulen, 512, &istatus);
if(!ulen||istatus<200||istatus>299)
	{
	DoAbort(&ssl, &net);
	return;
	}
while((istatus=mbedtls_ssl_handshake(&ssl))!=0)
	{
	if(istatus!=MBEDTLS_ERR_SSL_WANT_READ&&istatus!=MBEDTLS_ERR_SSL_WANT_WRITE)
		{
		DoAbort(&ssl, &net);
		return;
		}
	}
if(mbedtls_ssl_get_verify_result(&ssl)!=0)
	{
	DoAbort(&ssl, &net);
	return;
	}
ulen=StringPrint(pbuf, 512, "EHLO esp\r\n");
ulen=SendAndReceiveSsl(&ssl, pbuf, ulen, 512, &istatus);
if(!ulen||istatus<200||istatus>299)
	{
	DoAbort(&ssl, &net);
	return;
	}
ulen=StringPrint(pbuf, 512, "AUTH LOGIN\r\n");
ulen=SendAndReceiveSsl(&ssl, pbuf, ulen, 512, &istatus);
if(!ulen||istatus<200||istatus>399)
	{
	DoAbort(&ssl, &net);
	return;
	}
istatus=mbedtls_base64_encode(hbuf->Begin(), 128, &ulen, (BYTE const*)User->Begin(), User->GetLength());
if(istatus!=0)
	{
	DoAbort(&ssl, &net);
	return;
	}
pbuf[ulen++]='\r';
pbuf[ulen++]='\n';
pbuf[ulen]=0;
ulen=SendAndReceiveSsl(&ssl, pbuf, ulen, 512, &istatus);
if(!ulen||istatus<300||istatus>399)
	{
	DoAbort(&ssl, &net);
	return;
	}
istatus=mbedtls_base64_encode(hbuf->Begin(), 128, &ulen, (BYTE const*)Password->Begin(), Password->GetLength());
if(istatus!=0)
	{
	DoAbort(&ssl, &net);
	return;
	}
pbuf[ulen++]='\r';
pbuf[ulen++]='\n';
pbuf[ulen]=0;
ulen=SendAndReceiveSsl(&ssl, pbuf, ulen, 512, &istatus);
if(!ulen||istatus<200||istatus>399)
	{
	DoAbort(&ssl, &net);
	return;
	}
auto pfrom=hEmail->From->Begin();
ulen=StringPrint(pbuf, 512, "MAIL FROM:<%s>\r\n", pfrom);
ulen=SendAndReceiveSsl(&ssl, pbuf, ulen, 512, &istatus);
if(!ulen||istatus<200||istatus>299)
	{
	DoAbort(&ssl, &net);
	return;
	}
auto pto=hEmail->To->Begin();
ulen=StringPrint(pbuf, 512, "RCPT TO:<%s>\r\n", pto);
ulen=SendAndReceiveSsl(&ssl, pbuf, ulen, 512, &istatus);
if(!ulen||istatus<200||istatus>299)
	{
	DoAbort(&ssl, &net);
	return;
	}
ulen=StringPrint(pbuf, 512, "DATA\r\n");
ulen=SendAndReceiveSsl(&ssl, pbuf, ulen, 512, &istatus);
if(!ulen||istatus<300||istatus>399)
	{
	DoAbort(&ssl, &net);
	return;
	}
auto psubject=hEmail->Subject->Begin();
ulen=StringPrint(pbuf, 512, "From: %s\r\nSubject: %s\r\nTo: %s\r\n\r\n", pfrom, psubject, pto);
ulen=SendDataSsl(&ssl, pbuf, ulen);
if(!ulen)
	{
	DoAbort(&ssl, &net);
	return;
	}
auto ptext=hEmail->Text->Begin();
ulen=hEmail->Text->GetLength();
ulen=SendDataSsl(&ssl, ptext, ulen);
if(!ulen)
	{
	DoAbort(&ssl, &net);
	return;
	}
ulen=StringPrint(pbuf, 512, "\r\n.\r\n");
ulen=SendAndReceiveSsl(&ssl, pbuf, ulen, 512, &istatus);
if(!ulen||istatus<200||istatus>299)
	{
	DoAbort(&ssl, &net);
	return;
	}
mbedtls_ssl_close_notify(&ssl);
mbedtls_ssl_session_reset(&ssl);
mbedtls_net_free(&net);
Application::Current->Loop.AddProc(this, &SmtpClient::OnSent);
}

VOID SmtpClient::OnAborted()
{
Aborted(this);
}

VOID SmtpClient::OnSent()
{
Sent(this);
}

SIZE_T SmtpClient::ReceiveData(VOID* pnet, LPSTR pbuf, SIZE_T usize, INT* pstatus)
{
if(uStatus==SmtpClientStatus::Closing)
	return 0;
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

SIZE_T SmtpClient::ReceiveDataSsl(VOID* pssl, LPSTR pbuf, SIZE_T usize, INT* pstatus)
{
if(uStatus==SmtpClientStatus::Closing)
	return 0;
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

SIZE_T SmtpClient::SendAndReceive(VOID* pnet, LPSTR pbuf, SIZE_T usize, SIZE_T ubufsize, INT* pstatus)
{
*pstatus=-1;
if(!SendData(pnet, pbuf, usize))
	return 0;
return ReceiveData(pnet, pbuf, ubufsize, pstatus);
}

SIZE_T SmtpClient::SendAndReceiveSsl(VOID* pssl, LPSTR pbuf, SIZE_T usize, SIZE_T ubufsize, INT* pstatus)
{
*pstatus=-1;
if(!SendDataSsl(pssl, pbuf, usize))
	return 0;
return ReceiveDataSsl(pssl, pbuf, ubufsize, pstatus);
}

SIZE_T SmtpClient::SendData(VOID* pnet, LPCSTR pbuf, SIZE_T usize)
{
if(uStatus==SmtpClientStatus::Closing)
	return 0;
INT istatus=mbedtls_net_send((mbedtls_net_context*)pnet, (BYTE const*)pbuf, usize);
if(istatus<=0)
	return 0;
return (SIZE_T)istatus;
}

SIZE_T SmtpClient::SendDataSsl(VOID* pssl, LPCSTR pbuf, SIZE_T usize)
{
if(uStatus==SmtpClientStatus::Closing)
	return 0;
INT istatus=0;
while((istatus=mbedtls_ssl_write((mbedtls_ssl_context*)pssl, (BYTE const*)pbuf, usize))<=0)
	{
	if(istatus!= MBEDTLS_ERR_SSL_WANT_READ&&istatus!= MBEDTLS_ERR_SSL_WANT_WRITE)
		return 0;
	}
return (SIZE_T)istatus;
}

VOID SmtpClient::SendProc(VOID* pparam)
{
Handle<SmtpClient> hclient=(SmtpClient*)pparam;
hclient->hCallback=nullptr;
hclient->DoSend();
hclient->hEmail=nullptr;
hclient->uStatus=SmtpClientStatus::Closed;
vTaskDelete(NULL);
}

}}
