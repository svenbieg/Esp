//=============
// WiFiEvent.h
//=============

#pragma once


//===========
// Namespace
//===========

namespace Network {
	namespace WiFi {


//============
// WiFi-Event
//============

enum class WiFiEvent
{
ClientConnected,
ClientDisconnected,
StationConnected,
StationDisconnected,
StationStarted
};


//======================
// WiFi-Event-Arguments
//======================

struct WiFiEventArgs
{
UINT Gateway;
UINT Ip;
UINT Subnet;
};

}}