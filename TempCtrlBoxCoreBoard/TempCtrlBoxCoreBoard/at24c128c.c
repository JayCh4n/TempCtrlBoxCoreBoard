/*
 * at24c128c.c
 *
 * Created: 2018-06-23 16:47:24
 *  Author: 123
 */ 

#include "at24c128c.h"

void write_template_to_eeprom(uint8_t *template_point, uint8_t template_num)
{
	uint16_t i;
	
	uint16_t ee_begin_addr = (template_num - 1) * TEMPLATE_SIZE;
//	uint16_t ee_end_addr = (template_num - 1) * TEMPLATE_SIZE + TEMPLATE_SIZE - 1;
	
	for (i = 0; i < TEMPLATE_SIZE; i++)
	{
		at24c128c_write_byte(i + ee_begin_addr, *(template_point+i));
	}
}

void read_template_from_eeprom(uint8_t *template_point, uint8_t template_num)
{
	uint16_t i;
	
	uint16_t ee_begin_addr = (template_num - 1) * TEMPLATE_SIZE;
	
	for (i = 0; i < TEMPLATE_SIZE; i++)
	{
		*(template_point + i) = at24c128c_read_byte(i + ee_begin_addr);
	}
}

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
/*	_delay_us(60);*/
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