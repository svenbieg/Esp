//=============
// OtaUpdate.h
//=============

#pragma once


//=======
// Using
//=======

#include "Network/Http/HttpClient.h"
#include "Storage/Buffer.h"


//===========
// Namespace
//===========

namespace Network {
	namespace Ota {


//============
// OTA-Update
//============

class OtaUpdate: public Object
{
private:
	// Using
	using Buffer=Storage::Buffer;
	using HttpClient=Network::Http::HttpClient;
	using HttpConnection=Network::Http::HttpConnection;
	using HttpResponse=Network::Http::HttpResponse;

public:
	// Con-/Destructors
	OtaUpdate();

	// Common
	VOID Abort();
	VOID Begin(Handle<String> Url);
	Event<OtaUpdate> Failed;
	Event<OtaUpdate> Successful;
	Event<OtaUpdate, Handle<String>> Status;

private:
	// Common
	VOID OnHttpConnectionClosed(Handle<HttpConnection> Connection);
	VOID OnHttpClientConnected(Handle<HttpClient> Client, Handle<HttpConnection> Connection);
	VOID OnHttpClientConnectionFailed(Handle<HttpClient> Client);
	VOID OnHttpConnectionDataReceived(Handle<HttpConnection> Connection);
	VOID OnHttpConnectionResponse(Handle<HttpConnection> Connection, Handle<HttpResponse> Response);
	Handle<Buffer> hBuffer;
	Handle<HttpConnection> hConnection;
	Handle<HttpClient> hHttpClient;
	Handle<String> hUrl;
	VOID const* pPartition;
	UINT uImagePos;
	UINT uImageSize;
	UINT uUpdateHandle;
};

}}