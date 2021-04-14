//===============
// DnsServer.cpp
//===============

#include "pch.h"


//=======
// Using
//=======

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lwip/sockets.h>
#include "Network/WiFi/WiFiConnection.h"
#include "DnsHelper.h"
#include "DnsServer.h"

using namespace Network::WiFi;


//===========
// Namespace
//===========

namespace Network {
	namespace Dns {


//==================
// Con-/Destructors
//==================

DnsServer::DnsServer():
uStatus(DnsServerStatus::Closed)
{}


//========
// Common
//========

VOID DnsServer::Close()
{
if(uStatus==DnsServerStatus::Closed)
	return;
uStatus=DnsServerStatus::Closing;
while(uStatus!=DnsServerStatus::Closed)
	Sleep(10);
}

VOID DnsServer::Listen()
{
Close();
hCallback=this;
uStatus=DnsServerStatus::Listening;
xTaskCreatePinnedToCore(&DnsTask, "DnsTask", 4096, this, (2|portPRIVILEGE_BIT), nullptr, 0);
}


//================
// Common Private
//================

VOID DnsServer::DnsTask(VOID* pparam)
{
Handle<DnsServer> hserver=(DnsServer*)pparam;
hserver->hCallback=nullptr;
hserver->DoListen();
hserver->uStatus=DnsServerStatus::Closed;
vTaskDelete(nullptr);
}

WORD DnsServer::HandleRequest(BYTE* preq, WORD ureq, BYTE* pres)
{
if(ureq<sizeof(DnsHeader)||ureq>=DnsMaxMessageSize)
	return 0;
DnsHeader* prqhdr=(DnsHeader*)preq;
if(prqhdr->AdditionalCount||prqhdr->AnswerCount||prqhdr->AuthorityCount)
	return 0;
if(GetFlag(prqhdr->Flags, DnsFlags::Truncated))
	return 0;
DnsHeader* prshdr=(DnsHeader*)pres;
CopyMemory(pres, preq, ureq);
SetFlag(prshdr->Flags, DnsFlags::Response);
BYTE* pq=preq+sizeof(DnsHeader);
BYTE* pr=pres+ureq;
BYTE* pend=&pres[DnsMaxMessageSize];
WORD ucount=ReadWord(&prqhdr->QuestionCount);
WORD uawcount=0;
for(UINT u=0; u<ucount; u++)
	{
	CHAR plabel[64];
	pq=ReadLabel(preq, ureq, pq, plabel, DnsMaxMessageSize);
	if(pq==nullptr)
		return 0;
	DnsQuestionFooter* pqf=(DnsQuestionFooter*)pq;
	pq+=sizeof(DnsQuestionFooter);
	UINT utype=ReadWord(&pqf->Type);
	switch(utype)
		{
		case DnsQuestionType::A:
			{
			BOOL bredirect=false;
			if(StringCompare(plabel, "www.google.com", 0, false)==0)
				bredirect=true;
			if(StringCompare(HostName? HostName->Begin(): nullptr, plabel, 0, false)==0)
				bredirect=true;
			if(!bredirect)
				return 0;
			pr=WriteLabel(pres, DnsMaxMessageSize, pr, plabel);
			if(pr==nullptr)
				return 0;
			DnsResourceFooter* prf=(DnsResourceFooter*)pr;
			pr+=sizeof(DnsResourceFooter);
			if(pr+4>pend)
				return 0;
			ZeroMemory(prf, sizeof(DnsResourceFooter));
			WriteWord(&prf->Type, DnsQuestionType::A);
			WriteWord(&prf->Class, DnsQuestionClass::Internet);
			WriteWord(&prf->Size, 4);
			UINT ip=WiFiConnection::Current->AccessPointIp;
			BYTE* pip=(BYTE*)&ip;
			*pr++=pip[0];
			*pr++=pip[1];
			*pr++=pip[2];
			*pr++=pip[3];
			uawcount++;
			break;
			}
		}
	}
if(uawcount==0)
	return 0;
WriteWord(&prshdr->AnswerCount, uawcount);
return (WORD)(pr-pres);
}

VOID DnsServer::DoListen()
{
sockaddr_in addr;
ZeroMemory(&addr, sizeof(sockaddr_in));
addr.sin_addr.s_addr=INADDR_ANY;
addr.sin_family=AF_INET;
addr.sin_port=htons(DnsPort);
addr.sin_len=sizeof(sockaddr_in);
INT isocket=socket(AF_INET, SOCK_DGRAM, 0);
if(isocket==-1)
	{
	DebugPrint("DnsServer - socket() failed\n");
	return;
	}
INT iopt=1;
if(setsockopt(isocket, SOL_SOCKET, SO_REUSEADDR, &iopt, sizeof(INT))<0)
	{
	DebugPrint("DnsServer - setsockopt() failed\n");
	return;
	}
INT istatus=bind(isocket, (sockaddr const*)&addr, sizeof(sockaddr_in));
if(istatus!=0)
	{
	DebugPrint("DnsServer - bind() failed\n");
	return;
	}
BYTE preq[DnsMaxMessageSize];
BYTE pres[DnsMaxMessageSize];
while(uStatus==DnsServerStatus::Listening)
	{
	if(GetRefCount()==1)
		break;
	sockaddr_in from;
	socklen_t fromlen=sizeof(sockaddr_in);
	INT istatus=recvfrom(isocket, preq, DnsMaxMessageSize, 0, (sockaddr*)&from, &fromlen);
	if(istatus<0)
		istatus=0;
	WORD ures=HandleRequest(preq, istatus, pres);
	if(ures==0)
		{
		Sleep(10);
		continue;
		}
	istatus=sendto(isocket, pres, ures, 0, (sockaddr const*)&from, fromlen);
	if(istatus<0)
		{
		DebugPrint("DnsServer::sendto failed (%i)\n", istatus);
		}
	}
close(isocket);
}

BYTE* DnsServer::ReadLabel(BYTE* ppacket, WORD usize, BYTE* plabel, LPSTR pstr, WORD umaxlen)
{
BYTE* pend=&ppacket[usize];
BYTE* pnext=nullptr;
WORD ustrpos=0;
do
	{
	BYTE ulen=*plabel;
	if((ulen&0xC0)==0xC0)
		{
		pnext=plabel+2;
		WORD uoffset=ReadWord(plabel)&0x3FFF;
		if(uoffset>usize)
			return nullptr;
		plabel=&ppacket[uoffset];
		continue;
		}
	plabel++;
	if(ustrpos>0&&ustrpos+1<umaxlen)
		pstr[ustrpos++]='.';
	for(WORD u=0; u<ulen; u++)
		{
		if(plabel>=pend)
			return nullptr;
		if(ustrpos+1>=umaxlen)
			return nullptr;
		pstr[ustrpos++]=*plabel++;
		}
	}
while(*plabel!=0);
pstr[ustrpos]=0;
if(pnext==nullptr)
	pnext=plabel+1;
return pnext;
}

WORD DnsServer::ReadWord(VOID const* pbuf)
{
BYTE const* p=(BYTE const*)pbuf;
return ((WORD)p[0]<<8)|p[1];
}

BYTE* DnsServer::WriteLabel(BYTE* ppacket, WORD usize, BYTE* plabel, LPCSTR pstr)
{
BYTE* pend=&ppacket[usize];
BYTE* plen=plabel++;
while(plabel<pend)
	{
	if(*pstr=='.'||*pstr==0)
		{
		BYTE ulen=(BYTE)(plabel-plen)-1;
		if(ulen==0)
			return nullptr;
		*plen=ulen;
		if(*pstr==0)
			{
			*plabel++=0;
			return plabel;
			}
		plen=plabel++;
		pstr++;
		}
	else
		{
		*plabel++=*pstr++;
		}
	}
return nullptr;
}

VOID DnsServer::WriteWord(WORD* pbuf, WORD u)
{
BYTE* p=(BYTE*)pbuf;
*p++=(BYTE)(u>>8);
*p++=(BYTE)u;
}

}}
