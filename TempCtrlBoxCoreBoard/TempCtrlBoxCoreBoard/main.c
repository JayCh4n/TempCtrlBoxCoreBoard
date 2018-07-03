/*
* tempctrlboxmonitor_v1r0.c
*
* Created: 2018-04-03 09:05:18
* Author : chenlong
*/

#include <avr/io.h>
#include <avr/eeprom.h>
/*#include "delay.h"*/
#include "usart.h"
#include "timer.h"
#include "dgus.h"
#include "gpio.h"
#include "at24c128c.h"
#include <util/delay.h>

extern void read_eeprom_data(void);
void system_init(void);

int main(void)
{
	uint8_t slave_num = 1;
//   	template_struct_typedef t1;
//   	template_struct_typedef t2;
// 	uint8_t sta;
/*	uint32_t name1,name2,name3,name4;*/
	
// 	uint8_t t1[100];
// 	uint8_t t2[100];
	
	system_init();
	EN_INTERRUPT;
	read_eeprom_data();

	_delay_ms(1000);
	
// 	t1.sta = 0;
// 	t1.name = set_name[3];
	
// 	write_template_to_eeprom(t1, 1);
// 	read_template_from_eeprom(t2, 1);
	
// 	write_template_to_eeprom((uint8_t *)&t1, 1);
// 	read_template_from_eeprom((uint8_t *)&t2, 2);
/*	sta = read_sta_from_eeprom(1);*/
// 	name1 = read_name_from_eeprom(1);	
// 	name2 = read_name_from_eeprom(2);
// 	name3 = read_name_from_eeprom(3);
// 	name4 = read_name_from_eeprom(4);
	read_setting_data_all(); //开机从主控板读取设定数据

	//	send_variables(MASTER_SWITCH, pre_system_sta);	//默认系统为关闭状态 调节屏幕状态图标为关闭
	send_variables(ALL_SET_TEMP, all_temp);
	send_variables(SET_PREHEAT_TIME, preheat_time);
	send_variables(TEMP_UINT_ADDR, (CELSIUS + temp_unit * FAHRENHEIT));
	send_variables(ALL_SENSOR_TYPE_ADDR, TYPE_J + (all_sensor_type * TYPE_K));

	init_complete = 1;
	update_main_page();
	switch_language();

	/* Replace with your application code */
	while (1)
	{
		if (usart0_rx_end) //屏相关数据接收完成
		{
			usart0_deal();
			usart0_rx_end = 0;
		}

		if (usart1_rx_end) //控制卡数据接收完成
		{
			usart1_deal();
			usart1_rx_end = 0;
		}

		// 		if (in_main_page) //
		// 		{
		// 			update_main_page();
		// 		}

		/*500ms发送一次请求获取主控板卡运行数据*/
		// if (ctrl_command == READ_DATA_ALL)
		// {
		// 	if (usart1_tx_overtime_mask == 1)
		// 	{
		// 		send_request_all(slave_num);

		// 		if (++slave_num >= TEMP_CTRL_BOARD_QUANTITY + 1)
		// 		{
		// 			slave_num = 1;
		// 		}

		// 		usart1_tx_overtime_mask = 0;
		// 		usart1_tx_timecnt = 0;
		// 	}
		// }

		if (usart1_tx_overtime_mask == 1)
		{
			send_request_all(slave_num);
			if (++slave_num >= TEMP_CTRL_BOARD_QUANTITY + 1)
			{
				slave_num = 1;
			}
			
// 			else
// 			{
// 				switch (ctrl_command[ctrl_index - 1])
// 				{
// 					case PID:
// 					set_pid();
// 					ctrl_index--;
// 					break;
// 
// 					case TEMP:
// 					if (all_set_flag)
// 					{
// 						if (++all_set_cnt <= TEMP_CTRL_BOARD_QUANTITY)
// 						{
// 							all_set(TEMP, all_temp);
// 						}
// 						else
// 						{
// 							all_set_cnt = 0;
// 							all_set_flag = 0;
// 							ctrl_index--;
// 						}
// 					}
// 					else
// 					{
// 						single_set(TEMP, set_temp[set_num]);
// 						ctrl_index--;
// 					}
// 					break;
// 
// 					case PREHEAT_TIME:
// 					if (++all_set_cnt <= TEMP_CTRL_BOARD_QUANTITY)
// 					{
// 						all_set(PREHEAT_TIME, preheat_time);
// 					}
// 					else
// 					{
// 						all_set_cnt = 0;
// 						ctrl_index--;
// 					}
// 					break;
// 
// 					case SENSOR_TYPE:
// 					if (++all_set_cnt <= TEMP_CTRL_BOARD_QUANTITY)
// 					{
// 						all_set(SENSOR_TYPE, all_sensor_type);
// 					}
// 					else
// 					{
// 						all_set_cnt = 0;
// 						ctrl_index--;
// 					}
// 					break;
// 
// 					case SWITCH_SENSOR:
// 					if (all_set_flag)
// 					{
// 						if (++all_set_cnt <= TEMP_CTRL_BOARD_QUANTITY)
// 						{
// 							switch_all_sensor(pre_system_sta);
// 						}
// 						else
// 						{
// 							all_set_cnt = 0;
// 							all_set_flag = 0;
// 							ctrl_index--;
// 						}
// 					}
// 					else
// 					{
// 						single_set(SWITCH_SENSOR, switch_sensor[set_num]);
// 						ctrl_index--;
// 					}
// 					break;
// 
// 					case SET_FOLLOW:
// 					single_set(SET_FOLLOW, follow_sta[set_num]);
// 					ctrl_index--;
// 					break;
// 					default: break;
// 				}
// 			}
			usart1_tx_overtime_mask = 0;
			usart1_tx_timecnt = 0;
		}

		/*1.5秒监测一次告警状态*/
		if (alarm_monitor_overtime_mask)
		{
			alarm_monitor();
			alarm_monitor_overtime_mask = 0;
		}
	}
	
	return 0;
}

void system_init()
{
	gpio_init();
	usart0_init(MYUBRR(38400));
	usart1_init(MYUBRR(8928));
	usart2_init(9600);

	timer0_init();
	timer2_init();
	timer1_init();

	twi_init(400);

	init_variable();
}
