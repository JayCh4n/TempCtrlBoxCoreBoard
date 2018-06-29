/*
 * at24c128c.c
 *
 * Created: 2018-06-23 16:47:24
 *  Author: 123
 */

#include "at24c128c.h"

uint16_t template_eeaddr[MAX_TEMPLATE_QUANTITY];

void apply_template(uint8_t template_num)
{
	uint8_t i,j,k;
	
	read_template_from_eeprom((uint8_t *)&template_structure, template_num);
	
	for (i=0; i<MAX_IQR_QUANTITY; i++)
	{
		set_temp[i] = template_structure.set_temp[i];
		set_temp_buff[i] = set_temp[i];
		sensor_type[i] = template_structure.sensor_type[i];
		sensor_type_buff[i] = sensor_type[i];
	}

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 8; j++)
		{
			for (k = 0; k < 4; k++)
			{
				time_ctrl_value[i][j][k] = template_structure.ctrl_time[i][j][k];
				time_ctrl_value_buff[i][j][k] = time_ctrl_value[i][j][k];
				eeprom_write_word((uint16_t *)(TIME_CTRL_VALUE_EEADDR + (i * 32 + j * 4 + k) * 2), time_ctrl_value[i][j][k]);
			}
		}
	}	
}

/*搜索模板 返回值为第几个模板 0：未搜索到模板*/
uint8_t find_template(uint32_t name)
{
	uint8_t tp_num;
	
	for(tp_num = 1; tp_num <= template_cnt; tp_num++)
	{
		if(name == read_name_from_eeprom(tp_num))
		{
			return tp_num;
		}
	}
	
	return 0;
}

void set_template_name(uint8_t template_num, uint32_t name)
{
	if (template_num <= template_cnt)
	{
		write_name_to_eeprom(template_num, name);
	}
	update_template_page(pre_first_tpnum);
}

void template_del(uint8_t template_num)
{
	uint8_t i;
	uint16_t addr_temp;
	
	if (template_num <= template_cnt)
	{
		template_tip_msg = ICON_DELED;
		addr_temp = template_eeaddr[template_num-1];
		
		for(i = template_num; i<template_cnt; i++)
		{
			template_eeaddr[i-1] = template_eeaddr[i];
		}
		
		template_eeaddr[template_cnt-1] = addr_temp;
		template_cnt--;
		if(template_cnt < pre_first_tpnum)
		{
			if(template_cnt <= 5)
			{
				pre_first_tpnum = 1;
			}
			else
			{
				pre_first_tpnum = template_cnt - 4;
			}
		}
		update_template_page(pre_first_tpnum);
	}
	
	for (i = 0; i < MAX_TEMPLATE_QUANTITY; i++)
	{
		eeprom_write_word((uint16_t *)(TEMPLATE_EEADDR + i * 2), template_eeaddr[i]);
	}
	
	eeprom_write_byte((uint8_t*)TEMPLATE_CNT_EEADDR, template_cnt);	
}

void save_preset_to_template(uint8_t template_num)
{
	int8_t i, j, k;
	uint16_t addr_temp;
	
	template_tip_msg = ICON_SAVED;

	template_structure.sta = 0;
	template_structure.name = tp_save_name;

	for (i = 0; i < MAX_IQR_QUANTITY; i++)
	{
		template_structure.set_temp[i] = set_temp[i];
		template_structure.sensor_type[i] = sensor_type[i];
	}

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 8; j++)
		{
			for (k = 0; k < 4; k++)
			{
				template_structure.ctrl_time[i][j][k] = time_ctrl_value[i][j][k];
			}
		}
	}
	
	addr_temp = template_eeaddr[MAX_TEMPLATE_QUANTITY - 1];
	
	for (i = MAX_TEMPLATE_QUANTITY - 2; i >= 0; i--)
	{
		template_eeaddr[i + 1] = template_eeaddr[i];
	}

	template_eeaddr[0] = addr_temp;
	
	write_template_to_eeprom((uint8_t *)&template_structure, 1);
	
	pre_first_tpnum = 1;
	update_template_page(pre_first_tpnum);
	
	for (i = 0; i < MAX_TEMPLATE_QUANTITY; i++)
	{
		eeprom_write_word((uint16_t *)(TEMPLATE_EEADDR + i * 2), template_eeaddr[i]);
	}
	
	eeprom_write_byte((uint8_t*)TEMPLATE_CNT_EEADDR, template_num);
}

void write_sta_to_eeprom(uint8_t template_num, uint8_t sta)
{
	uint8_t addr;
	
	addr = template_eeaddr[template_num-1];
	
	at24c128c_write_byte(addr,sta);
}

uint8_t read_sta_from_eeprom(uint8_t template_num)
{
	uint8_t sta;
	uint16_t addr;

	addr = template_eeaddr[template_num-1];
	sta = at24c128c_read_byte(addr);

	return sta;
}

void write_name_to_eeprom(uint8_t template_num, uint32_t name)
{
	uint16_t addr;
	
	addr = template_eeaddr[template_num-1]+4;
	
	at24c128c_write_byte(addr, name>>24);
	at24c128c_write_byte(addr-1, name>>16);
	at24c128c_write_byte(addr-2, name>>8);
	at24c128c_write_byte(addr-3, name);
}

uint32_t read_name_from_eeprom(uint8_t template_num)
{
	uint16_t addr;
	uint32_t name;

	addr = template_eeaddr[template_num-1] + 4;

	name = at24c128c_read_byte(addr);
	name = name << 8 | at24c128c_read_byte(addr - 1);
	name = name << 8 | at24c128c_read_byte(addr - 2);
	name = name << 8 | at24c128c_read_byte(addr - 3);

	return name;
}

void write_template_to_eeprom(uint8_t *template_point, uint8_t template_num)
{
	uint16_t i;

	uint16_t ee_begin_addr = template_eeaddr[template_num-1];
	//	uint16_t ee_end_addr = (template_num - 1) * TEMPLATE_SIZE + TEMPLATE_SIZE - 1;

	for (i = 0; i < TEMPLATE_SIZE; i++)
	{
		at24c128c_write_byte(i + ee_begin_addr, *(template_point + i));
	}
}

void read_template_from_eeprom(uint8_t *template_point, uint8_t template_num)
{
	uint16_t i;

	uint16_t ee_begin_addr = template_eeaddr[template_num-1];

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
		if (++cnt >= 5)
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

	while (!twi_write_byte(addr_l))
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
	_delay_ms(2);
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

	_delay_ms(2);

	return data;
}