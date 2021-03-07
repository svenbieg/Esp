//===============
// Application.h
//===============

#pragma once


//=======
// Using
//=======

#include "Core/Application.h"
#include "Network/Ota/OtaUpdate.h"
#include "Storage/Collections/Log.h"
#include "Storage/Collections/Settings.h"
#include "Storage/AppData.h"


//===========
// Namespace
//===========

namespace Runtime {


//=============
// Application
//=============

class Application: public Core::Application
{
public:
	// Using
	using OtaUpdate=Network::Ota::OtaUpdate;

	// Common
	Handle<Storage::AppData> AppData;
	static Handle<Application> Current;
	Handle<Storage::Collections::Log> Log;
	Handle<Storage::Collections::Settings> Settings;
	VOID Update();
	Handle<StringVariable> UpdateStatus;

protected:
	// Con-/Destructors
	Application(Handle<String> Name, Handle<String> UpdateUrl=nullptr);

private:
	// Common
	VOID OnOtaUpdateFailed(Handle<OtaUpdate> Update);
	VOID OnOtaUpdateStatus(Handle<OtaUpdate> Update, Handle<String> Status);
	VOID OnOtaUpdateSuccessful(Handle<OtaUpdate> Update);
	Handle<OtaUpdate> hUpdate;
	Handle<String> hUpdateUrl;
};

}