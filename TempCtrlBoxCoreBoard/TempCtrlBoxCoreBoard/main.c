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
	
	system_init();
	EN_INTERRUPT;
	read_eeprom_data();

	_delay_ms(1000);
	
	read_setting_data_all(); //开机从主控板读取设定数据

	//	send_variables(MASTER_SWITCH, pre_system_sta);	//默认系统为关闭状态 调节屏幕状态图标为关闭
	send_variables(ALL_SET_TEMP, all_temp);
	send_variables(SET_PREHEAT_TIME, preheat_time);
	send_variables(TEMP_UINT_ADDR, (CELSIUS + temp_unit * FAHRENHEIT));
	send_variables(ALL_SENSOR_TYPE_ADDR, TYPE_J + (all_sensor_type * TYPE_K));
	send_variables(TIME_CTRL_MODE_ADDR, time_ctrl_mode);
	set_time_ctrl_mode(time_ctrl_mode);
	
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

		if (usart1_tx_overtime_mask == 1)
		{
			send_request_all(slave_num);
			if (++slave_num >= TEMP_CTRL_BOARD_QUANTITY + 1)
			{
				slave_num = 1;
			}
			
			usart1_tx_overtime_mask = 0;
			usart1_tx_timecnt = 0;
		}

		/*1.5秒监测一次告警状态*/
		if (alarm_monitor_overtime_mask)
		{
			alarm_monitor();
			alarm_monitor_overtime_mask = 0;
			
// 			if(screen_protection_over_time_mask)
// 			{
// 				enter_screen_protection();
// 			}
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
