//===================
// OneWireHelper.cpp
//===================

#include "pch.h"


//=======
// Using
//=======

#include <driver/gpio.h>
#include "Mutex.h"
#include "OneWireHelper.h"


//===========
// Namespace
//===========

namespace Devices {


//========
// Common
//========

BOOL IRAM_ATTR OneWireReadBit(BYTE upin)
{
Mutex mutex;
gpio_set_direction((gpio_num_t)upin, GPIO_MODE_OUTPUT);
gpio_set_level((gpio_num_t)upin, 0);
SleepMicroseconds(6);
gpio_set_direction((gpio_num_t)upin, GPIO_MODE_INPUT);
gpio_set_level((gpio_num_t)upin, 1);
SleepMicroseconds(9);
INT ivalue=gpio_get_level((gpio_num_t)upin);
SleepMicroseconds(55);
return (ivalue&0x01)>0;
}

BOOL IRAM_ATTR OneWireReset(BYTE upin)
{
gpio_set_direction((gpio_num_t)upin, GPIO_MODE_INPUT);
UINT uretry=128;
while(!gpio_get_level((gpio_num_t)upin))
	{
	if(--uretry==0)
		return false;
	}
Mutex mutex;
gpio_set_direction((gpio_num_t)upin, GPIO_MODE_OUTPUT);
SleepMicroseconds(0);
gpio_set_level((gpio_num_t)upin, 0);
SleepMicroseconds(480);
gpio_set_direction((gpio_num_t)upin, GPIO_MODE_INPUT);
gpio_set_level((gpio_num_t)upin, 1);
SleepMicroseconds(70);
INT ivalue=gpio_get_level((gpio_num_t)upin);
SleepMicroseconds(410);
return (ivalue&0x01)==0;
}

VOID IRAM_ATTR OneWireWriteBit(BYTE upin, BOOL bvalue)
{
UINT udelay1=bvalue? 6: 60;
UINT udelay2=bvalue? 64: 10;
Mutex mutex;
gpio_set_direction((gpio_num_t)upin, GPIO_MODE_OUTPUT);
gpio_set_level((gpio_num_t)upin, 0);
SleepMicroseconds(udelay1);
gpio_set_level((gpio_num_t)upin, 1);
SleepMicroseconds(udelay2);
}

}