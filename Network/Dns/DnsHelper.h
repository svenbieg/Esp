//=============
// DnsHelper.h
//=============

#pragma once


//===========
// Namespace
//===========

namespace Network {
	namespace Dns {


//=====
// Dns
//=====

const WORD DnsMaxMessageSize=512;
const WORD DnsPort=53;

enum class DnsFlags: BYTE
{
Response=0x80,
Truncated=0x02
};

struct DnsHeader
{
WORD Id;
DnsFlags Flags;
BYTE Code;
WORD QuestionCount;
WORD AnswerCount;
WORD AuthorityCount;
WORD AdditionalCount;
}__attribute__((packed));

struct DnsQuestionFooter
{
WORD Type;
WORD Class;
};

enum DnsQuestionType: WORD
{
A=1,
NS=2,
URI=256
};

enum DnsQuestionClass: WORD
{
Internet=1
};

struct DnsResourceFooter
{
WORD Type;
WORD Class;
UINT TimeToLive;
WORD Size;
}__attribute__((packed));

}}