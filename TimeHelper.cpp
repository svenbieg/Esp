//================
// TimeHelper.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_timer.h>


//========
// Common
//========

UINT GetTickCount()
{
return (UINT)(esp_timer_get_time()/1000);
}

UINT64 GetTickCount64()
{
return esp_timer_get_time()/1000;
}

UINT64 GetTickCountUs()
{
return esp_timer_get_time();
}

VOID Sleep(UINT umillis)
{
vTaskDelay(umillis*configTICK_RATE_HZ/1000);
}

VOID SleepMicroseconds(UINT uus)
{
ets_delay_us(uus);
}
