//============
// I2CBus.cpp
//============

#include "pch.h"


//=======
// Using
//=======

#include <driver/i2c.h>
#include "I2CBus.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace I2C {


//==================
// Con-/Destructors
//==================

I2CBus::I2CBus(BYTE sda, BYTE scl):
pCommand(nullptr)
{
esp_err_t status=i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER
#ifdef ESP32
, 0, 0, 0
#endif
);
if(status!=ESP_OK)
	{
	DebugPrint("i2c_driver_install failed (%u)\n", status);
	}
i2c_config_t cfg;
ZeroMemory(&cfg, sizeof(i2c_config_t));
cfg.mode=I2C_MODE_MASTER;
cfg.scl_io_num=(gpio_num_t)scl;
cfg.sda_io_num=(gpio_num_t)sda;
#ifdef ESP32
cfg.master.clk_speed=1000000;
#endif
status=i2c_param_config(I2C_NUM_0, &cfg);
if(status!=ESP_OK)
	{
	DebugPrint("i2c_param_config failed (%u)\n", status);
	}
}

I2CBus::~I2CBus()
{
i2c_driver_delete(I2C_NUM_0);
}


//==============
// Input-Stream
//==============

SIZE_T I2CBus::Available()
{
return 0;
}

SIZE_T I2CBus::Read(VOID* pbuf, SIZE_T usize)
{
if(pCommand==nullptr)
	return 0;
esp_err_t status=i2c_master_read((i2c_cmd_handle_t)pCommand, (BYTE*)pbuf, usize, I2C_MASTER_ACK);
if(status!=ESP_OK)
	{
	DebugPrint("i2c_master_read failed (%u)\n", status);
	return 0;
	}
return usize;
}


//===============
// Output-Stream
//===============

VOID I2CBus::Flush()
{
if(pCommand==nullptr)
	return;
esp_err_t status=i2c_master_stop((i2c_cmd_handle_t)pCommand);
if(status!=ESP_OK)
	{
	DebugPrint("i2c_master_stop failed (%u)\n", status);
	}
i2c_master_cmd_begin(I2C_NUM_0, (i2c_cmd_handle_t)pCommand, 0);
}

SIZE_T I2CBus::Write(BYTE uvalue)
{
if(pCommand==nullptr)
	return 0;
esp_err_t status=i2c_master_write_byte((i2c_cmd_handle_t)pCommand, uvalue, true);
if(status!=ESP_OK)
	{
	DebugPrint("i2c_master_write_byte failed (%u)\n", status);
	return 0;
	}
return 1;
}

SIZE_T I2CBus::Write(VOID const* pbuf, SIZE_T usize)
{
if(pCommand==nullptr)
	return 0;
esp_err_t status=i2c_master_write((i2c_cmd_handle_t)pCommand, (BYTE*)pbuf, usize, true);
if(status!=ESP_OK)
	{
	DebugPrint("i2c_master_write failed (%u)\n", status);
	return 0;
	}
return usize;
}


//========
// Common
//========

VOID I2CBus::BeginRead(BYTE uaddr)
{
if(pCommand!=nullptr)
	{
	DebugPrint("I2CBus::BeginRead - Already transmitting\n");
	return;
	}
pCommand=i2c_cmd_link_create();
if(pCommand==nullptr)
	{
	DebugPrint("I2CBus::BeginRead - i2c_cmd_link_create failed\n");
	return;
	}
esp_err_t status=i2c_master_start((i2c_cmd_handle_t)pCommand);
if(status!=ESP_OK)
	{
	DebugPrint("I2CBus::BeginRead - i2c_master_start failed (%u)\n", status);
	}
BYTE ucmd=(uaddr<<1)|I2C_MASTER_READ;
status=i2c_master_write_byte((i2c_cmd_handle_t)pCommand, ucmd, true);
if(status!=ESP_OK)
	{
	DebugPrint("I2CBus::BeginRead - i2c_master_write_byte failed (%u)\n", status);
	}
}

BOOL I2CBus::BeginWrite(BYTE uaddr)
{
if(pCommand!=nullptr)
	{
	DebugPrint("I2CBus::BeginWrite - Already transmitting\n");
	return false;
	}
pCommand=i2c_cmd_link_create();
if(pCommand==nullptr)
	{
	DebugPrint("I2CBus::BeginWrite - i2c_cmd_link_create failed\n");
	return false;
	}
esp_err_t status=i2c_master_start((i2c_cmd_handle_t)pCommand);
if(status!=ESP_OK)
	{
	DebugPrint("I2CBus::BeginWrite - i2c_master_start failed (%u)\n", status);
	return false;
	}
BYTE ucmd=(uaddr<<1)|I2C_MASTER_WRITE;
status=i2c_master_write_byte((i2c_cmd_handle_t)pCommand, ucmd, true);
if(status!=ESP_OK)
	{
	DebugPrint("I2CBus::BeginWrite - i2c_master_write_byte failed (%u)\n", status);
	return false;
	}
return true;
}

Handle<I2CBus> I2CBus::Current;

VOID I2CBus::EndTransmission()
{
if(pCommand==nullptr)
	return;
Flush();
i2c_cmd_link_delete((i2c_cmd_handle_t)pCommand);
pCommand=nullptr;
}

}}
