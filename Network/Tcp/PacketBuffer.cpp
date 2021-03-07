//==================
// PacketBuffer.cpp
//==================

#include "pch.h"


//=======
// Using
//=======

#include <lwip/pbuf.h>
#include <lwip/tcp.h>
#include "PacketBuffer.h"


//===========
// Namespace
//===========

namespace Network {
	namespace Tcp {


//==================
// Con-/Destructors
//==================

PacketBuffer::PacketBuffer():
pBuffer(nullptr),
uPosition(0)
{}

PacketBuffer::~PacketBuffer()
{
Clear();
}


//========
// Access
//========

WORD PacketBuffer::Available()
{
if(pBuffer==nullptr)
	return 0;
pbuf* pb=(pbuf*)pBuffer;
return pb->tot_len-uPosition;
}

BOOL PacketBuffer::Contains(VOID const* pcmpv, SIZE_T size)
{
BYTE const* pcmp=(BYTE const*)pcmpv;
pbuf* chunk=(pbuf*)pBuffer;
while(chunk)
	{
	BYTE const* pchunk=(BYTE const*)chunk->payload;
	WORD chunk_len=chunk->len;
	for(WORD chunk_pos=0; chunk_pos<chunk_len; chunk_pos++)
		{
		WORD available=chunk_len-chunk_pos;
		WORD compare=(WORD)MIN(size, available);
		if(CompareMemory(&pchunk[chunk_pos], pcmp, compare)==0)
			{
			if(compare==size)
				return true;
			pbuf* next=chunk->next;
			if(!next)
				return false;
			BYTE const* pnext=(BYTE const*)next->payload;
			return CompareMemory(pnext, &pcmp[compare], size-compare)==0;
			}
		}
	chunk=chunk->next;
	}
return false;
}

WORD PacketBuffer::Read(VOID* pdst, WORD usize)
{
if(!pBuffer)
	return 0;
pbuf* pb=(pbuf*)pBuffer;
BYTE* pd=(BYTE*)pdst;
WORD utotal=pb->tot_len-uPosition;
WORD ucopy=MIN(usize, utotal);
WORD upos=0;
while(ucopy>0)
	{
	pb=(pbuf*)pBuffer;
	WORD uread=pb->len-uPosition;
	uread=MIN(uread, ucopy);
	if(pd)
		{
		BYTE* psrc=(BYTE*)pb->payload;
		CopyMemory(&pd[upos], &psrc[uPosition], uread);
		}
	//BOOL bend=(pb->len==pb->tot_len);
	Consume(uread);
	upos+=uread;
	ucopy-=uread;
	/*if(bend)
		break;*/
	}
return upos;
}


//==============
// Modification
//==============

WORD PacketBuffer::Append(VOID* pappend)
{
pbuf* pa=(pbuf*)pappend;
if(pBuffer==nullptr)
	{
	pBuffer=pa;
	return pa->tot_len;
	}
pbuf* pb=(pbuf*)pBuffer;
pbuf_cat(pb, pa);
return pb->tot_len;
}

VOID PacketBuffer::Clear()
{
if(pBuffer==nullptr)
	return;
pbuf_free((pbuf*)pBuffer);
pBuffer=nullptr;
uPosition=0;
}


//================
// Common Private
//================

WORD PacketBuffer::Consume(WORD usize)
{
if(pBuffer==nullptr)
	return 0;
WORD upos=0;
while(usize>0)
	{
	pbuf* pb=(pbuf*)pBuffer;
	WORD ulen=pb->len;
	if(usize<ulen-uPosition)
		{
		uPosition+=usize;
		upos+=usize;
		return upos;
		}
	pbuf* pnext=pb->next;
	if(pnext==nullptr)
		{
		upos+=ulen;
		pbuf_free(pb);
		pBuffer=nullptr;
		uPosition=0;
		return upos;
		}
	WORD uconsume=ulen-uPosition;
	pbuf_ref(pnext);
	pbuf_free(pb);
	pBuffer=pnext;
	uPosition=0;
	upos+=uconsume;
	usize-=uconsume;
	}
return upos;
}

}}
