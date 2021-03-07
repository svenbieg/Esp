//==============
// TaskHelper.h
//==============

#pragma once


//=======
// Using
//=======

#include "Procedure.h"


//========
// Common
//========

VOID CreateTask(Procedure Procedure, LPCSTR Name="", UINT StackSize=4096, UINT Priority=5);
VOID Yield();
