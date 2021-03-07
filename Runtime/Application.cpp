//=================
// Application.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include "Application.h"


//===========
// Namespace
//===========

namespace Runtime {


//========
// Common
//========

Handle<Application> Application::Current;

VOID Application::Update()
{
if(!hUpdateUrl)
	return;
hUpdate=new OtaUpdate();
hUpdate->Failed.Add(this, &Application::OnOtaUpdateFailed);
hUpdate->Status.Add(this, &Application::OnOtaUpdateStatus);
hUpdate->Successful.Add(this, &Application::OnOtaUpdateSuccessful);
hUpdate->Begin(hUpdateUrl);
}


//============================
// Con-/Destructors Protected
//============================

Application::Application(Handle<String> hname, Handle<String> hurl):
Core::Application(hname),
hUpdateUrl(hurl)
{
Current=this;
AppData=new Storage::AppData();
Log=new Storage::Collections::Log();
Settings=new Storage::Collections::Settings();
UpdateStatus=new StringVariable("UpdateStatus");
}


//================
// Common Private
//================

VOID Application::OnOtaUpdateFailed(Handle<OtaUpdate> hupdate)
{
hUpdate=nullptr;
}

VOID Application::OnOtaUpdateStatus(Handle<OtaUpdate> hupdate, Handle<String> hstatus)
{
UpdateStatus=hstatus;
}

VOID Application::OnOtaUpdateSuccessful(Handle<OtaUpdate> hupdate)
{
Log->Write("Update installiert");
hUpdate=nullptr;
}

}