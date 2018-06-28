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
#include "at24c128c.h"

/*当前语言 告警 射胶阀控制时间参数*/
void read_eeprom_data(void)
{
	uint8_t i,j,k;

	if (eeprom_read_byte((uint8_t *)FIRST_START_ADDR) != 'y')
	{
		eeprom_write_byte((uint8_t *)PRE_LANGUAGE_EEADDR, 0); //当前语言 与开机时切换到的界面有关
		eeprom_write_byte((uint8_t *)ALARM_CNT_EEADDR, 0);	//告警清单数量
		
		for(i=0;i<4;i++)
		{
			for(j=0;j<8;j++)
			{
				for (k=0;k<4;k++)
				{
					eeprom_write_word((uint16_t *)(TIME_CTRL_VALUE_EEADDR + (i * 32 + j * 4 + k) * 2), time_ctrl_value[i][j][k]);
				}
			}
		}
		
		for(i=0; i<MAX_TEMPLATE_QUANTITY; i++)
		{
			template_eeaddr[i] = i * TEMPLATE_SIZE;
			eeprom_write_word((uint16_t *)(TEMPLATE_EEADDR + i * 2), template_eeaddr[i]);
		}

		eeprom_write_byte((uint8_t *)FIRST_START_ADDR, 'y');
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

		for (i = 0; i < MAX_TEMPLATE_QUANTITY; i++)
		{
			template_eeaddr[i] = eeprom_read_word((uint16_t *)(TEMPLATE_EEADDR + i * 2));
		}

		for(i=0;i<4;i++)
		{
			for(j=0;j<8;j++)
			{
				for (k=0;k<4;k++)
				{
					time_ctrl_value[i][j][k] = eeprom_read_word((uint16_t *)(TIME_CTRL_VALUE_EEADDR + (i * 32 + j * 4 + k) * 2));
					time_ctrl_value_buff[i][j][k] = time_ctrl_value[i][j][k];
				}
			}
		}
				
	}
}
