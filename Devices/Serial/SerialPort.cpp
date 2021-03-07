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
uId(uid),
uTimeout(1000)
{
esp_err_t status=uart_driver_install((uart_port_t)uid, 256, 256, 0, nullptr, 0);
if(status!=ESP_OK)
	{
	printf("uart_driver_install failed (%u)\r\n", status);
	return;
	}
status=uart_set_baudrate((uart_port_t)uId, (UINT)baud);
if(status!=ESP_OK)
	{
	printf("uart_set_baudrate failed (%u)\r\n", status);
	return;
	}
}

SerialPort::~SerialPort()
{
uart_driver_delete((uart_port_t)uId);
}


//========
// Common
//========

Handle<SerialPort> SerialPort::Current;


//=======
// Input
//=======

SIZE_T SerialPort::Available()
{
return 0;
}

SIZE_T SerialPort::Read(VOID* pbuf, SIZE_T usize)
{
return uart_read_bytes((uart_port_t)uId, (BYTE*)pbuf, usize, 1000);
}


//========
// Output
//========

SIZE_T SerialPort::AvailableForWrite()
{
return 0;
}

VOID SerialPort::Flush()
{
uart_flush((uart_port_t)uId);
}

VOID SerialPort::Print(LPCSTR pstr)
{
UINT ulen=StringLength(pstr);
uart_write_bytes((uart_port_t)uId, pstr, ulen);
uart_flush((uart_port_t)uId);
}

VOID SerialPort::Print(Handle<String> hstr)
{
if(hstr==nullptr)
	return;
Print(hstr->Begin());
}

VOID SerialPort::Print(UINT ulen, LPCSTR pstr)
{
ulen=StringLength(pstr, ulen);
uart_write_bytes((uart_port_t)uId, pstr, ulen);
uart_flush((uart_port_t)uId);
}

SIZE_T SerialPort::Write(VOID const* pbuf, SIZE_T usize)
{
return uart_write_bytes((uart_port_t)uId, (LPCSTR)pbuf, usize);
}

}}
