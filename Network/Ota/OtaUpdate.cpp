//===============
// OtaUpdate.cpp
//===============

#include "pch.h"


//=======
// Using
//=======

#include <esp_image_format.h>
#include <esp_ota_ops.h>
#include "Storage/Streams/StreamReader.h"
#include "OtaUpdate.h"

using namespace Network::Http;
using namespace Storage::Streams;


//===========
// Namespace
//===========

namespace Network {
	namespace Ota {


//==================
// Con-/Destructors
//==================

OtaUpdate::OtaUpdate():
pPartition(nullptr),
uImagePos(0),
uImageSize(0),
uUpdateHandle(0)
{}


//========
// Common
//========

VOID OtaUpdate::Abort()
{
hBuffer=nullptr;
if(hConnection)
	{
	hConnection->Closed.Remove(this);
	hConnection->Close();
	hConnection=nullptr;
	}
hHttpClient=nullptr;
hUrl=nullptr;
pPartition=nullptr;
uImagePos=0;
uImageSize=0;
if(uUpdateHandle)
	{
	esp_ota_end((esp_ota_handle_t)uUpdateHandle);
	uUpdateHandle=0;
	}
}

VOID OtaUpdate::Begin(Handle<String> hurl)
{
Abort();
hUrl=hurl;
hHttpClient=new HttpClient();
hHttpClient->Connected.Add(this, &OtaUpdate::OnHttpClientConnected);
hHttpClient->ConnectionFailed.Add(this, &OtaUpdate::OnHttpClientConnectionFailed);
UINT upath=0;
StringFind(hurl->Begin(), "/", &upath);
Handle<String> hhost=new String(upath, hurl->Begin());
hHttpClient->Connect(hhost);
Status(this, "Verbinden...");
}


//================
// Common Private
//================

VOID OtaUpdate::OnHttpConnectionClosed(Handle<HttpConnection> hcon)
{
DebugPrint("OtaUpdate - Connection closed\n");
Status(this, "Verbindung unterbrochen");
Abort();
Failed(this);
}

VOID OtaUpdate::OnHttpClientConnected(Handle<HttpClient> hclient, Handle<HttpConnection> hcon)
{
DebugPrint("OtaUpdate - Connected\n");
Status(this, "Verbunden");
hConnection=hcon;
hConnection->Closed.Add(this, &OtaUpdate::OnHttpConnectionClosed);
hConnection->DataReceived.Add(this, &OtaUpdate::OnHttpConnectionDataReceived);
hConnection->Response.Add(this, &OtaUpdate::OnHttpConnectionResponse);
Handle<HttpRequest> hrequest=HttpRequest::FromUrl(hUrl);
hConnection->Send(hrequest);
}

VOID OtaUpdate::OnHttpClientConnectionFailed(Handle<HttpClient> hclient)
{
DebugPrint("OtaUpdate - Connection failed\n");
Status(this, "Verbindung fehlgeschlagen");
Abort();
Failed(this);
}

VOID OtaUpdate::OnHttpConnectionDataReceived(Handle<HttpConnection> hcon)
{
auto uota=(esp_ota_handle_t)uUpdateHandle;
auto ppart=(esp_partition_t const*)pPartition;
StreamReader reader(hcon);
BYTE* pbuf=hBuffer->Begin();
SIZE_T usize=hcon->Available();
while(usize>0)
	{
	SIZE_T ucopy=MIN(uImageSize, 4096);
	ucopy=MIN(ucopy, usize);
	SIZE_T uread=reader.Read(pbuf, ucopy);
	if(!ppart)
		{
		#ifdef ESP32
		if(uread<sizeof(esp_image_header_t)+sizeof(esp_image_segment_header_t)+sizeof(esp_app_desc_t))
			{
			Abort();
			Successful(this);
			return;
			}
		BYTE* ptmp=pbuf;
		ptmp+=sizeof(esp_image_header_t);
		ptmp+=sizeof(esp_image_segment_header_t);
		esp_app_desc_t const* pcur=esp_ota_get_app_description();
		esp_app_desc_t const* pnew=(esp_app_desc_t*)ptmp;
		DebugPrint("OtaUpdate - Current: %s - New: %s\n", pcur->version, pnew->version);
		if(StringCompare(pcur->version, pnew->version)==0)
			{
			Status(this, "Aktuell");
			Abort();
			Failed(this);
			return;
			}
		#endif
		ppart=esp_ota_get_next_update_partition(NULL);
		if(!ppart)
			{
			DebugPrint("esp_ota_get_next_update_partition() failed\n");
			Status(this, "Fehler Partition");
			Abort();
			Failed(this);
			return;
			}
		if(esp_ota_begin(ppart, uImageSize, &uota)!=ESP_OK)
			{
			DebugPrint("esp_ota_begin() failed\n");
			Status(this, "Fehler Initialisierung");
			Abort();
			Failed(this);
			return;
			}
		pPartition=ppart;
		uUpdateHandle=uota;
		}
	if(esp_ota_write(uota, pbuf, uread)!=ESP_OK)
		{
		DebugPrint("esp_ota_write() failed\n");
		Status(this, "Fehler Schreiben");
		Abort();
		Failed(this);
		return;
		}
	usize-=uread;
	uImagePos+=uread;
	}
if(uImagePos<uImageSize)
	{
	INT istatus=uImagePos*100/uImageSize;
	Status(this, new String("%i%%", istatus));
	return;
	}
uUpdateHandle=0;
if(esp_ota_end(uota)!=ESP_OK)
	{
	DebugPrint("esp_ota_end() failed\n");
	Status(this, "Fehler Beenden");
	Abort();
	Failed(this);
	return;
	}
Abort();
esp_err_t status=esp_ota_set_boot_partition(ppart);
if(status!=ESP_OK)
	{
	DebugPrint("esp_ota_set_boot_partition failed (%i)\n", (INT)status);
	Status(this, "Fehler Bootloader");
	Failed(this);
	return;
	}
Status(this, "Erfolgreich");
Successful(this);
}

VOID OtaUpdate::OnHttpConnectionResponse(Handle<HttpConnection> hcon, Handle<HttpResponse> hresponse)
{
if(hresponse->Status!=HttpStatus::Ok)
	{
	LPCSTR pstatus=HttpStatusToString(hresponse->Status);
	DebugPrint("OtaUpdate - Response Status (%u - %s)\n", (UINT)hresponse->Status, pstatus);
	Status(this, "Fehler");
	Abort();
	Failed(this);
	return;
	}
auto hcontentlen=hresponse->Properties->Get("Content-Length");
if(!hcontentlen)
	{
	DebugPrint("OtaUpdate - No length\n");
	Status(this, "Fehler");
	Abort();
	Failed(this);
	}
if(!hcontentlen->Scan("%u", &uImageSize))
	{
	DebugPrint("OtaUpdate - Invalid length\n");
	Status(this, "Fehler");
	Abort();
	Failed(this);
	return;
	}
hBuffer=new Buffer(4096);
uImagePos=0;
}

}}