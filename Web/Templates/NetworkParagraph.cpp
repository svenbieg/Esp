//======================
// NetworkParagraph.cpp
//======================

#include "pch.h"


//=======
// Using
//=======

#include "Network/WiFi/WiFiConnection.h"
#include "Web/Controls/Tables/Table.h"
#include "Web/Controls/Button.h"
#include "Web/Controls/Caption.h"
#include "Web/Controls/EditBox.h"
#include "Web/Controls/Label.h"
#include "Web/Controls/TextBlock.h"
#include "NetworkParagraph.h"

using namespace Network::WiFi;
using namespace Web::Controls;
using namespace Web::Controls::Tables;


//===========
// Namespace
//===========

namespace Web {
	namespace Templates {


//==================
// Con-/Destructors
//==================

NetworkParagraph::NetworkParagraph()
{
auto hcon=WiFiConnection::Current;
hStationNetwork=new StringVariable("StationNetwork", hcon->StationNetwork);
hStationPassword=new StringVariable("StationPassword", hcon->StationPassword);
new Caption("Netzwerk", 2);
Handle<Table> htable=new Table("simple");
	new Row(); new Cell("static"); new TextBlock("Ip-Adresse"); new Cell("num"); new Label(hcon->StationIp);
	new Row(); new Cell("static"); new TextBlock("Gateway"); new Cell("num"); new Label(hcon->StationGateway);
	new Row(); new Cell("static"); new TextBlock("Subnetz"); Handle<Cell> hcell=new Cell("num"); new Label(hcon->StationSubnet);
		hcell->Style="width:100px";
htable->Break();
this->Select();
Handle<Button> hbtn;
Handle<EditBox> hedit;
htable=new Table("blank");
	new Row(); new Cell("right"); new TextBlock("SSID"); new Cell(); hedit=new EditBox(hStationNetwork);
		hedit->Style="margin-left:4px; width:200px";
	new Row(); new Cell("right"); new TextBlock("Passwort"); new Cell(); hedit=new EditBox(hStationPassword);
		hedit->InputType=EditBoxInputType::Password;
		hedit->Style="margin-left:4px; margin-top:4px; width:200px";
	new Row(); new Cell(); new Cell(); hbtn=new Button("NetworkConnect", "Verbinden");
		hbtn->Clicked.Add(this, &NetworkParagraph::OnConnectClick);
		hbtn->Style="margin-top:4px";
Parent->Select();
}


//================
// Common Private
//================

VOID NetworkParagraph::OnConnectClick(Handle<Button> hbtn)
{
auto hcon=WiFiConnection::Current;
hcon->StationNetwork=hStationNetwork->Get();
hcon->StationPassword=hStationPassword->Get();
hcon->Connect();
}

}}