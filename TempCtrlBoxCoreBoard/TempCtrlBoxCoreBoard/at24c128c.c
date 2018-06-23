/*
 * at24c128c.c
 *
 * Created: 2018-06-23 16:47:24
 *  Author: 123
 */ 

#include "at24c128c.h"

/*at24c128c写字节 返回值：0失败 1成功*/
uint8_t at24c128c_write_byte(uint16_t addr, uint8_t data)
{
	uint8_t addr_h, addr_l;
	uint8_t cnt = 0;
	
	addr_h = addr >> 8;
	addr_l = addr;
	
	while (!twi_start())
	{
		if(++cnt >= 5)
		{
			twi_stop();
			return 0;
		}
	}
	
	while (!twi_adrsing(AT24C128C_ADDR, WRITE))
	{
		if (++cnt >= 5)
		{
			twi_stop();
			return 0;
		}
	}
	
	while (!twi_write_byte(addr_h))
	{
		if (++cnt >= 5)
		{
			twi_stop();
			return 0;
		}
	}
	
	while(!twi_write_byte(addr_l))
	{
		if (++cnt >= 5)
		{
			twi_stop();
			return 0;
		}
	}
	
	while (!twi_write_byte(data))
	{
		if (++cnt >= 5)
		{
			twi_stop();
			return 0;
		}
	}
	
	twi_stop();
	return 1;
}

/*at24c128c读字节 0-255：读取值  0xFFFF：失败*/
uint16_t at24c128c_read_byte(uint16_t addr)
{
	uint8_t data;
	uint8_t addr_h, addr_l;
	uint8_t cnt = 0;
	
	addr_h = addr >> 8;
	addr_l = addr;
	
	while (!twi_start())
	{
		if (++cnt >= 5)
		{
			twi_stop();
			return 0xFFFF;
		}
	}
	
	while (!twi_adrsing(AT24C128C_ADDR, WRITE))
	{
		if (++cnt >= 5)
		{
			twi_stop();
			return 0xFFFF;
		}
	}
	
	while (!twi_write_byte(addr_h))
	{
		if (++cnt >= 5)
		{
			twi_stop();
			return 0xFFFF;
		}
	}
	
	while (!twi_write_byte(addr_l))
	{
		if (++cnt >= 5)
		{
			twi_stop();
			return 0xFFFF;
		}
	}
	
	while (!twi_start())
	{
		if (++cnt >= 5)
		{
			twi_stop();
			return 0xFFFF;
		}
	}
	
	while (!twi_adrsing(AT24C128C_ADDR, READ))
	{
		if (++cnt >= 5)
		{
			twi_stop();
			return 0xFFFF;
		}
	}
	
	while ((data = twi_read_byte(NACK)) == 0xFFFF)
	{
		if (++cnt >= 5)
		{
			twi_stop();
			return 0xFFFF;
		}
	}
	
	twi_stop();

	return data;
}