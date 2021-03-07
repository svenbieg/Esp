//=================
// BlockDevice.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include "Devices/Storage/BlockDevice.h"
#include "FlashDevice.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Storage {


//========
// Common
//========

Handle<BlockDevice> BlockDevice::GetDefault()
{
return new FlashDevice();
}

}}