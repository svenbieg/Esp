//==================
// WiFiConnection.h
//==================

#pragma once


//=======
// Using
//=======

#include "Network/Dns/DnsServer.h"
#include "Network/Connection.h"
#include "Core/Timer.h"
#include "WiFiEvent.h"


//===========
// Namespace
//===========

namespace Network {
	namespace WiFi {


//=================
// WiFi-Connection
//=================

class WiFiConnection: public Connection
{
private:
	// Using
	using DnsServer=Network::Dns::DnsServer;
	using IpAddress=Network::IpAddress;
	using Timer=Core::Timer;

public:
	// Con-/Destructors
	WiFiConnection();

	// Common
	Handle<IpAddress> AccessPointIp;
	Handle<String> AccessPointNetwork;
	Handle<String> AccessPointPassword;
	Handle<IpAddress> AccessPointSubnet;
	static BYTE Channel;
	VOID Close();
	VOID Connect();
	Event<WiFiConnection> Connected;
	static Handle<WiFiConnection> Current;
	VOID Disconnect();
	Event<WiFiConnection> Disconnected;
	BOOL IsConnected()const { return StationIp->Get()!=0; }
	VOID Notify(WiFiEvent Event, WiFiEventArgs* Args);
	Handle<StringVariable> StationNetwork;
	Handle<StringVariable> StationPassword;

private:
	// Common
	VOID OnLoop();
	VOID OnTimerTriggered(Handle<Timer> Timer);
	Handle<String> hAccessPointNetwork;
	Handle<DnsServer> hDnsServer;
	Handle<Timer> hTimer;
	UINT uStationGateway;
	UINT uStationIp;
	UINT uStationSubnet;
};

}}