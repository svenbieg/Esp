//================
// TaskHelper.cpp
//================

#include "pch.h"

//=======
// Using
//=======

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_task_wdt.h>
#include "TaskHelper.h"


//================
// Task-Procedure
//================

VOID TaskProc(VOID* pparam)
{
auto pproc=(Procedure*)pparam;
pproc->Run();
delete pproc;
vTaskDelete(NULL);
}


//========
// Common
//========

VOID CreateTask(Procedure proc, LPCSTR pname, UINT ustack, UINT uprio)
{
auto pproc=new Procedure(proc);
xTaskCreate(TaskProc, pname, ustack, pproc, uprio, nullptr);
}

VOID Yield()
{
esp_task_wdt_reset();
}
