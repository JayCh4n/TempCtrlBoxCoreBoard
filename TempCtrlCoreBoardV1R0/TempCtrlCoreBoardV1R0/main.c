/*
* tempctrlboxmonitor_v1r0.c
*
* Created: 2018-04-03 09:05:18
* Author : chenlong
*/

#include <avr/io.h>
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "dgus.h"
#include "gpio.h"
#include "eeprom.h"

void system_init(void);

int main(void)
{
	uint8_t slave_num = 1;
	
	system_init();
	EN_INTERRUPT;
	read_eeprom_data();

	_delay_ms(300);
	
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
		if (ctrl_command == READ_DATA_ALL)
		{
			if(usart1_tx_overtime_mask == 1)
			{
				send_request_all(slave_num);
				
				if(++slave_num >=4)
				{
					slave_num = 1;
				}
				
				usart1_tx_overtime_mask = 0;
				usart1_tx_timecnt = 0;
			}
		}
		
		/*1.5秒监测一次告警状态*/
		if(alarm_monitor_overtime_mask)
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
	usart0_init(MYUBRR(9600));
	usart1_init(MYUBRR(8928));
	usart2_init(9600);

	timer0_init();
	timer2_init();
	timer1_init();
	init_time_ctrl_value();
}
