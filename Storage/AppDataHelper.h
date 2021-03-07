//=================
// AppDataHelper.h
//=================

#pragma once


//=======
// Using
//=======

#include "Storage/Directory.h"


//===========
// Namespace
//===========

namespace Storage {


//========
// Common
//========

Handle<Directory> GetAppDataLocal();
Handle<Directory> GetAppDataNetwork();

}