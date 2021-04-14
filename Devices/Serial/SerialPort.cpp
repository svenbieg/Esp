//================
// SerialPort.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include <freertos/FreeRTOS.h>
#include <driver/uart.h>
#include "SerialPort.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Serial {


//==================
// Con-/Destructors
//==================

SerialPort::SerialPort(UINT uid, BaudRate baud):
pQueue(nullptr),
uId(uid)
{
uart_config_t config;
config.baud_rate=(UINT)baud;
config.data_bits=UART_DATA_8_BITS;
config.flow_ctrl=UART_HW_FLOWCTRL_DISABLE;
config.parity=UART_PARITY_DISABLE;
config.rx_flow_ctrl_thresh=0;
#ifdef ESP32
config.source_clk=UART_SCLK_APB;
#endif
config.stop_bits=UART_STOP_BITS_1;
esp_err_t status=uart_param_config((uart_port_t)uid, &config);
if(status!=ESP_OK)
	{
	DebugPrint("uart_param_config failed (%u)\r\n", status);
	return;
	}
#ifdef ESP32
status=uart_set_pin(uId, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
if(status!=ESP_OK)
	{
	DebugPrint("uart_set_pin failed (%u)\r\n", status);
	return;
	}
#endif
QueueHandle_t pqueue=nullptr;
status=uart_driver_install((uart_port_t)uId, 256, 256, 20, &pqueue, 0);
if(status!=ESP_OK)
	{
	DebugPrint("uart_driver_install failed (%u)\r\n", status);
	return;
	}
if(!pqueue)
	{
	DebugPrint("uart_driver_install: no queue\n");
	uart_driver_delete((uart_port_t)uId);
	return;
	}
#ifdef ESP32
status=uart_set_sw_flow_ctrl(uId, false, 0, 0);
if(status!=ESP_OK)
	{
	DebugPrint("uart_set_sw_flow_ctrl failed (%u)\r\n", status);
	return;
	}
#endif
pQueue=pqueue;
}

SerialPort::~SerialPort()
{
StopListening();
uart_driver_delete((uart_port_t)uId);
}


//========
// Common
//========

VOID SerialPort::ClearBuffer()
{
ScopedLock lock(cCriticalSection);
uart_flush_input((uart_port_t)uId);
}

VOID SerialPort::Listen()
{
ScopedLock lock(cCriticalSection);
if(hTask||!pQueue)
	return;
hTask=new ListenTask(this, &SerialPort::Listen, 2048, 12);
hTask->Run();
}

VOID SerialPort::StopListening()
{
ScopedLock lock(cCriticalSection);
if(!hTask)
	return;
hTask->Cancel=true;
hTask->Wait();
hTask=nullptr;
}


//=======
// Input
//=======

SIZE_T SerialPort::Available()
{
ScopedLock lock(cCriticalSection);
SIZE_T available=0;
if(uart_get_buffered_data_len((uart_port_t)uId, &available)!=ESP_OK)
	return 0;
return available;
}

SIZE_T SerialPort::Read(VOID* pbuf, SIZE_T usize)
{
ScopedLock lock(cCriticalSection);
return uart_read_bytes((uart_port_t)uId, (BYTE*)pbuf, usize, 0);
}


//========
// Output
//========

VOID SerialPort::Flush()
{
ScopedLock lock(cCriticalSection);
uart_flush((uart_port_t)uId);
}

SIZE_T SerialPort::Write(VOID const* pbuf, SIZE_T usize)
{
ScopedLock lock(cCriticalSection);
return uart_write_bytes((uart_port_t)uId, (LPCSTR)pbuf, usize);
}


//================
// Common Private
//================

VOID SerialPort::DoListen()
{
while(!hTask->Cancel)
	{
	uart_event_t event;
	if(xQueueReceive((QueueHandle_t)pQueue, (VOID*)&event, (portTickType)portMAX_DELAY))
		{
		switch(event.type)
			{
			case UART_DATA:
				{
				DataReceived(this);
				break;
				}
			default:
				break;
			}
		}
	}
}

}}
