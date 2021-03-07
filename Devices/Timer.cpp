//===========
// Timer.cpp
//===========

#include "pch.h"


//=======
// Using
//=======

#include <esp_attr.h>
#include <esp_timer.h>
#include "Timer.h"


//===========
// Namespace
//===========

namespace Devices {


//==================
// Con-/Destructors
//==================

Timer::Timer():
pTimer(nullptr)
{}

Timer::~Timer()
{
Stop();
}


//========
// Common
//========

VOID Timer::StartOnce(UINT uus)
{
Stop();
esp_timer_create_args_t args;
ZeroMemory(&args, sizeof(esp_timer_create_args_t));
args.arg=this;
args.callback=TimerProc;
esp_timer_handle_t ptimer=nullptr;
if(esp_timer_create(&args, &ptimer)!=ESP_OK)
	return;
hCallback=this;
pTimer=ptimer;
esp_timer_start_once(ptimer, uus);
}

VOID Timer::StartPeriodic(UINT uus)
{
Stop();
esp_timer_create_args_t args;
ZeroMemory(&args, sizeof(esp_timer_create_args_t));
args.arg=this;
args.callback=TimerProc;
esp_timer_handle_t ptimer=nullptr;
if(esp_timer_create(&args, &ptimer)!=ESP_OK)
	return;
hCallback=this;
pTimer=ptimer;
esp_timer_start_periodic(ptimer, uus);
}

VOID Timer::Stop()
{
if(!pTimer)
	return;
esp_timer_handle_t ptimer=(esp_timer_handle_t)pTimer;
esp_timer_stop(ptimer);
esp_timer_delete(ptimer);
hCallback=nullptr;
pTimer=nullptr;
}


//================
// Common Private
//================

VOID Timer::TimerProc(VOID* pparam)
{
Handle<Timer> htimer=(Timer*)pparam;
htimer->Triggered(htimer);
}

}