//================
// GpioHelper.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include <driver/gpio.h>
#include "GpioHelper.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Gpio {


//========
// Common
//========

BOOL DigitalRead(BYTE upin)
{
return gpio_get_level((gpio_num_t)upin);
}

VOID DigitalWrite(BYTE upin, BOOL b)
{
gpio_set_level((gpio_num_t)upin, b);
}

VOID SetPinMode(BYTE upin, PinMode mode)
{
gpio_config_t cfg;
ZeroMemory(&cfg, sizeof(gpio_config_t));
cfg.pin_bit_mask=1ULL<<upin;
cfg.mode=(mode==PinMode::Output? GPIO_MODE_OUTPUT: GPIO_MODE_INPUT);
cfg.pull_up_en=(mode==PinMode::InputPullup)? GPIO_PULLUP_ENABLE: GPIO_PULLUP_DISABLE;
esp_err_t err=gpio_config(&cfg);
if(err!=ESP_OK)
	{
	DebugPrint("gpio_config failed (%u)\n", (UINT)err);
	}
}


//============
// Interrupts
//============

UINT uInterruptPins=0;

VOID AttachInterrupt(BYTE upin, VOID (*pproc)(VOID*), VOID* parg)
{
if(!uInterruptPins)
	gpio_install_isr_service(ESP_INTR_FLAG_EDGE|ESP_INTR_FLAG_IRAM);
uInterruptPins++;
gpio_config_t conf;
ZeroMemory(&conf, sizeof(gpio_config_t));
conf.intr_type=GPIO_INTR_ANYEDGE;
conf.pin_bit_mask=1ULL<<upin;
conf.mode=GPIO_MODE_INPUT;
gpio_config(&conf);
gpio_isr_handler_add((gpio_num_t)upin, pproc, parg);
}

VOID DetachInterrupt(BYTE upin)
{
gpio_config_t conf;
ZeroMemory(&conf, sizeof(gpio_config_t));
conf.pin_bit_mask=1ULL<<upin;
gpio_config(&conf);
if(--uInterruptPins==0)
	gpio_install_isr_service(ESP_INTR_FLAG_INTRDISABLED);
}

}}
