/*
* eeprom.c
*
* Created: 2018-04-11 17:30:44
*  Author: chenlong
*/

#include <avr/eeprom.h>
#include <avr/io.h>
#include "dgus.h"
#include "usart.h"


void read_eeprom_data(void)
{
	uint8_t i;

	if (eeprom_read_byte((uint8_t *)FIRST_START_ADDR) != 'y')
	{
		eeprom_write_byte((uint8_t *)FIRST_START_ADDR, 'y');

		eeprom_write_byte((uint8_t *)PRE_LANGUAGE_EEADDR, 0); //当前语言 与开机时切换到的界面有关
		eeprom_write_byte((uint8_t *)ALARM_CNT_EEADDR, 0);	//告警清单数量
	}
	else
	{
		pre_language = eeprom_read_byte((uint8_t *)PRE_LANGUAGE_EEADDR);
		alarm_cnt = eeprom_read_byte((uint8_t *)ALARM_CNT_EEADDR);

		for (i = 0; i < alarm_cnt; i++)
		{
			alarm_history[i].alarm_type = eeprom_read_byte((uint8_t *)(ALARM_HISTORY_EEADDR + i * 2 + 1));
			alarm_history[i].alarm_device_num = eeprom_read_byte((uint8_t *)(ALARM_HISTORY_EEADDR + i * 2));
		}
	}
}
