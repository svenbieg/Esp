//====================
// WiFiConnection.cpp
//====================

#include "pch.h"


//=======
// Using
//=======

#include <esp_sleep.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include "Devices/ClockHelper.h"
#include "Network/WiFi/WiFiHelper.h"
#include "Runtime/Application.h"
#include "WiFiConnection.h"

using namespace Devices;
using namespace Runtime;


//===========
// Namespace
//===========

namespace Network {
	namespace WiFi {


//==================
// Con-/Destructors
//==================

WiFiConnection::WiFiConnection():
uStationGateway(0),
uStationIp(0),
uStationSubnet(0)
{
Current=this;
AccessPointIp=new IpAddress("WiFiAccessPointIp", 10, 0, 1, 1);
AccessPointSubnet=new IpAddress("WiFiAccessPointSubnet", 255, 255, 255, 0);
StationNetwork=new StringVariable("WiFiStationNetwork");
StationPassword=new StringVariable("WiFiStationPassword");
auto happ=Application::Current;
happ->Settings->Add(StationNetwork);
happ->Settings->Add(StationPassword);
if(nvs_flash_init()!=ESP_OK)
	{
	DebugPrint("nvs_flash_init() failed\n");
	return;
	}
WiFiInit();
happ->Loop.AddProc(this, &WiFiConnection::Connect);
}


//========
// Common
//========

BYTE WiFiConnection::Channel=0;

VOID WiFiConnection::Close()
{
WiFiClose();
hDnsServer=nullptr;
}

VOID WiFiConnection::Connect()
{
esp_wifi_disconnect();
wifi_mode_t mode=WIFI_MODE_NULL;
if(AccessPointNetwork)
	{
	mode=StationNetwork? WIFI_MODE_APSTA: WIFI_MODE_AP;
	}
else if(StationNetwork)
	{
	mode=WIFI_MODE_STA;
	}
if(esp_wifi_set_mode(mode)!=ESP_OK)
	{
	DebugPrint("esp_wifi_set_mode() failed\n");
	return;
	}
if(!AccessPointNetwork&&!StationNetwork)
	{
	Disconnect();
	return;
	}
if(AccessPointNetwork!=hAccessPointNetwork)
	{
	hAccessPointNetwork=AccessPointNetwork;
	if(AccessPointNetwork)
		{
		WiFiAccessPointConfig(AccessPointNetwork, AccessPointPassword, AccessPointIp, AccessPointIp, AccessPointSubnet);
		}
	else
		{
		if(hDnsServer)
			{
			hDnsServer->Close();
			hDnsServer=nullptr;
			}
		WiFiAccessPointClose();
		}
	}
if(StationNetwork)
	{
	WiFiStationConfig(StationNetwork, StationPassword);
	}
else
	{
	WiFiStationClose();
	}
if(esp_wifi_start()!=ESP_OK)
	{
	DebugPrint("esp_wifi_start() failed\n");
	return;
	}
#ifdef ESP32
if(StationNetwork)
	{
	if(HostName)
		WiFiStationSetHostName(HostName);
	if(esp_wifi_connect()!=ESP_OK)
		{
		DebugPrint("esp_wifi_connect() failed\n");
		return;
		}
	if(!hTimer)
		{
		hTimer=new Timer();
		hTimer->Triggered.Add(this, &WiFiConnection::OnTimerTriggered);
		hTimer->StartPeriodic(5000);
		}
	}
#endif
}

Handle<WiFiConnection> WiFiConnection::Current;

VOID WiFiConnection::Disconnect()
{
esp_wifi_disconnect();
WiFiAccessPointClose();
esp_wifi_stop();
hAccessPointNetwork=nullptr;
if(hDnsServer)
	{
	hDnsServer->Close();
	hDnsServer=nullptr;
	}
if(hTimer)
	{
	hTimer->Stop();
	hTimer=nullptr;
	}
}

VOID WiFiConnection::Notify(WiFiEvent event, WiFiEventArgs* pargs)
{
switch(event)
	{
	case WiFiEvent::ClientConnected:
		{
		hDnsServer=new DnsServer();
		hDnsServer->HostName=HostName;
		hDnsServer->Listen();
		break;
		}
	case WiFiEvent::ClientDisconnected:
		{
		hDnsServer=nullptr;
		break;
		}
	case WiFiEvent::StationConnected:
		{
		if(hTimer)
			{
			hTimer->Stop();
			hTimer=nullptr;
			}
		uStationGateway=pargs->Gateway;
		uStationIp=pargs->Ip;
		uStationSubnet=pargs->Subnet;
		Application::Current->Loop.AddProc(this, &WiFiConnection::OnLoop);
		break;
		}
	case WiFiEvent::StationDisconnected:
		{
		uStationGateway=0;
		uStationIp=0;
		uStationSubnet=0;
		Application::Current->Loop.AddProc(this, &WiFiConnection::OnLoop);
		break;
		}
	case WiFiEvent::StationStarted:
		{
		#ifdef ESP8266
		if(HostName)
			WiFiStationSetHostName(HostName->Begin());
		if(esp_wifi_connect()!=ESP_OK)
			{
			DebugPrint("esp_wifi_connect() failed\n");
			break;
			}
		if(!hTimer)
			{
			hTimer=new Timer();
			hTimer->Triggered.Add(this, &WiFiConnection::OnTimerTriggered);
			hTimer->StartPeriodic(5000);
			}
		#endif
		break;
		}
	}
}


//================
// Common Private
//================

VOID WiFiConnection::OnLoop()
{
if(uStationIp!=0)
	{
	StationIp->Set(uStationIp);
	StationGateway->Set(uStationGateway);
	StationSubnet->Set(uStationSubnet);
	DebugPrint("WiFi connected: %s\n", StationIp->ToString()->Begin());
	Connected(this);
	ClockStartSync();
	}
else
	{
	if(StationIp->Get()!=0)
		{
		DebugPrint("WiFi disconnected\n");
		StationIp->Set(0);
		StationGateway->Set(0);
		StationSubnet->Set(0);
		ClockStopSync();
		Disconnected(this);
		Connect();
		}
	}
}

VOID WiFiConnection::OnTimerTriggered(Handle<Timer> htimer)
{
Connect();
}

}}