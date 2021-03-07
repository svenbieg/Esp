//=================
// OneWireHelper.h
//=================

#pragma once


//=======
// Using
//=======

#include <esp_attr.h>


//===========
// Namespace
//===========

namespace Devices {


//========
// Common
//========

BOOL IRAM_ATTR OneWireReadBit(BYTE Pin);
BOOL IRAM_ATTR OneWireReset(BYTE Pin);
VOID IRAM_ATTR OneWireWriteBit(BYTE Pin, BOOL Value);

}