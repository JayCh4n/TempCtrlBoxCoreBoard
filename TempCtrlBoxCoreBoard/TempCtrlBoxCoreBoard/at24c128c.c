/*
 * at24c128c.c
 *
 * Created: 2018-06-23 16:47:24
 *  Author: 123
 */

#include "at24c128c.h"

uint16_t template_eeaddr[30];

void save_preset_to_template(uint8_t template_num)
{
	uint8_t i, j, k;
	uint8_t name1;
	uint16_t name2;
	uint16_t addr_temp;

	if (template_num < 9)
	{
		name2 = template_num % 10 + 0x30;
		name1 = 0;
	}
	else if (template_num >= 9 && template_num < 99)
	{
		name2 = template_num / 10 + 0x30;
		name1 = template_num % 10 + 0x30;
	}

	template_structure.sta = 0;
	template_structure.name = 0x54500000 | (name2 << 8) | name1;

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

	for (i = MAX_TEMPLATE_QUANTITY - 1; i <= 0; i++)
	{
		template_eeaddr[i] = template_eeaddr[i + 1];
	}

	template_eeaddr[0] = addr_temp;

	for (i = 0; i < MAX_TEMPLATE_QUANTITY; i++)
	{
		eeprom_write_word((uint16_t *)(TEMPLATE_EEADDR + i * 2), template_eeaddr[i]);
	}

	write_template_to_eeprom((uint8_t *)&template_structure, 1);
	
	pre_first_tpnum = 1;
	update_template_page(pre_first_tpnum);

}

uint8_t read_sta_from_eeprom(uint8_t template_num)
{
	uint8_t sta;
	uint16_t addr;

	addr = template_eeaddr[template_num-1];
	sta = at24c128c_read_byte(addr);

	return sta;
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