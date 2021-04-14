//===============
// TaskClass.cpp
//===============

#include "pch.h"


//=======
// Using
//=======

#include "TaskClass.h"


//========
// Common
//========

VOID Task::Wait()
{
while(hThis)
	Sleep(10);
}


//============================
// Con-/Destructors Protected
//============================

Task::Task():
Cancel(false)
{}
