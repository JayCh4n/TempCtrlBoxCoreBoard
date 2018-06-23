/*
* dgus.c
*
* Created: 2018-04-04 09:46:43
*  Author: chenlong
*/

#include "dgus.h"

uint8_t run_temp_page = 0;
uint8_t update_run_temp_flag = 0;
uint8_t in_main_page = 1;	//当前是否在主界面，刷新主页面数据标志位 0：否 1：是
uint8_t pre_language = 0;	//当前语言
int8_t pre_main_page = 0;   //当前主页面页号码
uint16_t pre_system_sta = 0; //当前系统状态
uint8_t set_num = 0;		 //单独设定键传感器位号
uint8_t set_pid_channel = 0;
int8_t curve_page_num = 0;   //曲线界面号
int8_t curve_time_level = 0; //曲线时间轴大小
uint8_t pid_page_num = 0;	//pid页面号


uint8_t ctrl_board_sta[TEMP_CTRL_BOARD_QUANTITY] = {0}; //控制板卡状态  0:断线 1:连接

uint8_t output_rate[MAX_IQR_QUANTITY] = {0};	//获取
uint8_t sensor_sta[MAX_IQR_QUANTITY] = {0};		//获取
uint8_t pre_sensor_sta[MAX_IQR_QUANTITY] = {0}; //上次监测时传感器状态 用来识别状态是否变化
int16_t run_temp[MAX_IQR_QUANTITY] = {0};		//获取

int16_t set_temp[MAX_IQR_QUANTITY]; //发送
uint16_t switch_sensor[MAX_IQR_QUANTITY] = {0};
uint16_t sensor_type[MAX_IQR_QUANTITY] = {0};
uint32_t set_name[MAX_IQR_QUANTITY];
uint32_t set_name_buff = 0;

uint8_t follow_sta[MAX_IQR_QUANTITY] = {0}; //通道跟随状态  0：禁止 1~24：跟随通道号
uint8_t follow_sta_buff[MAX_IQR_QUANTITY] = {0};

uint16_t p_value[MAX_IQR_QUANTITY]; //发送
uint16_t i_value[MAX_IQR_QUANTITY]; //发送
uint16_t d_value[MAX_IQR_QUANTITY]; //发送

uint16_t preheat_time = 3;	//发送
uint16_t all_sensor_type = 0; //发送
uint16_t temp_unit = 0;		  //
uint16_t all_temp = 100;	  //发送

uint8_t module_num = 1;			   //射胶模块号码
uint8_t module_status[4] = {0};	//射胶模块运行状态
uint16_t time_ctrl_value[4][8][4]; //射胶时间控制数据   共四个模块  每个模块8个通道  每个通道有 T1 T2 T3 T4 4个时间段

uint8_t alarm_cnt = 0;
alarm_struct_typedef alarm_history[MAX_ALARM_HISTORY];

uint8_t alarm_msg[7][16] = {
	{0xC8, 0xC8, 0xB5, 0xE7, 0xC5, 0xBC, 0xB6, 0xCF, 0xBF, 0xAA}, //热电偶断开
	{0xCE, 0xC2, 0xB6, 0xC8, 0xB9, 0xFD, 0xB8, 0xDF},			  //温度过高
	{0xCE, 0xC2, 0xB6, 0xC8, 0xB9, 0xFD, 0xB5, 0xCD},			  //温度过低
	{0xC8, 0xC8, 0xB5, 0xE7, 0xC5, 0xBC, 0xB6, 0xCC, 0xC2, 0xB7}, //热电偶短路
	{0xC8, 0xC8, 0xB5, 0xE7, 0xC5, 0xBC, 0xBD, 0xD3, 0xB7, 0xB4}, //热电偶接反
	{0xBC, 0xD3, 0xC8, 0xC8, 0xC6, 0xF7, 0xB6, 0xCF, 0xBF, 0xAA}, //加热器断开
	{0xBC, 0xD3, 0xC8, 0xC8, 0xC6, 0xF7, 0xB6, 0xCC, 0xC2, 0xB7}, //加热器短路
};

void init_variable(void)
{
	uint8_t i, j, k;
	uint8_t name1=0, name2=0, name3=0;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 8; j++)
		{
			for (k = 0; k < 4; k++)
			{
				time_ctrl_value[i][j][k] = 500; //射胶控制时间所有数据初始化为
				time_ctrl_value_buff[i][j][k] = 500;
			}
		}
	}

	for (i = 0; i < MAX_IQR_QUANTITY; i++)
	{
		set_temp[i] = 100;
		set_temp_buff[i] = 100;
		p_value[i] = 100;
		i_value[i] = 100;
		d_value[i] = 100;

		if (i < 9)
		{
			name3 = (i + 1) % 10 + 0x30;
			name2 = 0;
			name1 = 0;
		}
		else if (i >= 9 && i < 99)
		{
			name3 = (i + 1) / 10 + 0x30;
			name2 = (i + 1) % 10 + 0x30;
			name1 = 0;
		}
		else if (i >= 99 && i < 199)
		{
			name3 = (i + 1) / 100 + 0x30;
			name2 = (i + 1) / 10 + 0x30;
			name1 = (i + 1) % 10 + 0x30;
		}
		set_name[i] |= 0x23;		//固定前面显示#
		set_name[i] = (set_name[i] << 8) | name3;
		set_name[i] = (set_name[i] << 8) | name2;
		set_name[i] = (set_name[i] << 8) | name1;
	}
}

// void init_time_ctrl_value(void)
// {
// 	uint8_t i, j, k;

// 	for (i = 0; i < 4; i++)
// 	{
// 		for (j = 0; j < 8; j++)
// 		{
// 			for (k = 0; k < 4; k++)
// 			{
// 				time_ctrl_value[i][j][k] = 500; //射胶控制时间所有数据初始化为
// 				time_ctrl_value_buff[i][j][k] = 500;
// 			}
// 		}
// 	}
// }

void update_main_page(void)
{
	uint8_t i = 0;

	for (i = 0; i < IQR_QUANTITY_PER_PAGE; i++)
	{
		send_variables(MAINPAGE_NUM1_ADDR + (i * 2), (i + (pre_main_page * IQR_QUANTITY_PER_PAGE) + 1));		//主页面传感器序号显示
		send_variables(MAIN_OUTRATE1_ADDR + (i * 2), output_rate[i + (pre_main_page * IQR_QUANTITY_PER_PAGE)]); //传感器输出比例
		send_variables(MAIN_RUNTEMP1_ADDR + (i * 2), run_temp[i + (pre_main_page * IQR_QUANTITY_PER_PAGE)] +
														 temp_unit * (run_temp[i + (pre_main_page * IQR_QUANTITY_PER_PAGE)] * 8 / 10 + 32)); //运行温度
		send_variables(MAIN_SETTEMP1_ADDR + (i * 2), set_temp[i + (pre_main_page * IQR_QUANTITY_PER_PAGE)]);								 //设定温度
		send_variables(MAIN_SENSOR1_TYPE_ADDR + (i * 2), TYPE_J + (sensor_type[i + (pre_main_page * IQR_QUANTITY_PER_PAGE)]) * TYPE_K);

		send_name(MAIN_NAME1_ADDR + (i * 8), set_name[i + (pre_main_page * IQR_QUANTITY_PER_PAGE)]);

		/*更新传感状态图标 sensor_sta >= 3 时显示告警 0~2：关闭 开启 跟随*/
		/*更新告警类型图标 sensor_sta >= 3 时显示对应各种告警类型  0~2：显示运行正常*/
		if (sensor_sta[i + (pre_main_page * IQR_QUANTITY_PER_PAGE)] >= 3)
		{
			send_variables(MAIN_STA1_ADDR + (i * 4), 3);
			send_variables(MAIN_ALARM1_ADDR + (i * 8), sensor_sta[i + (pre_main_page * IQR_QUANTITY_PER_PAGE)]);
		}
		else
		{
			send_variables(MAIN_STA1_ADDR + (i * 4), sensor_sta[i + (pre_main_page * IQR_QUANTITY_PER_PAGE)]);
			send_variables(MAIN_ALARM1_ADDR + (i * 8), 2);
		}
	}
}

void key_action(uint16_t key_code)
{
	static uint8_t alarm_page_num = 1;
	uint8_t max_alarm_page_num;

	if (alarm_cnt == 0)
	{
		max_alarm_page_num = 1;
	}
	else if (alarm_cnt % 7 == 0)
	{
		max_alarm_page_num = alarm_cnt / 7;
	}
	else
	{
		max_alarm_page_num = alarm_cnt / 7 + 1;
	}

	switch (key_code)
	{
	case SWITCH_LANGUAGE:
		pre_language ^= 0x01;
		switch_language();
		break;
	case CLEAR_ALARM_SOUND:
		ALARM_OFF;
		break;
	case MAIN_PAGE_UP:
		if (--pre_main_page <= 0)
			pre_main_page = 0;
		update_main_page();
		break;
	case MAIN_PAGE_DOWN:
		if (++pre_main_page >= MAX_PAGE_QUANTITY-1)
			pre_main_page = MAX_PAGE_QUANTITY-1;
		update_main_page();
		break;
	case MAIN_SENSOR1_SET:
		set_num = 0 + (pre_main_page * IQR_QUANTITY_PER_PAGE);
		update_single_set_page();
		break;
	case MAIN_SENSOR2_SET:
		set_num = 1 + (pre_main_page * IQR_QUANTITY_PER_PAGE);
		update_single_set_page();
		break;
	case MAIN_SENSOR3_SET:
		set_num = 2 + (pre_main_page * IQR_QUANTITY_PER_PAGE);
		update_single_set_page();
		break;
	case MAIN_SENSOR4_SET:
		set_num = 3 + (pre_main_page * IQR_QUANTITY_PER_PAGE);
		update_single_set_page();
		break;
	case MAIN_SENSOR5_SET:
		set_num = 4 + (pre_main_page * IQR_QUANTITY_PER_PAGE);
		update_single_set_page();
		break;
	case MAIN_SENSOR6_SET:
		set_num = 5 + (pre_main_page * IQR_QUANTITY_PER_PAGE);
		update_single_set_page();
		break;
	case SINGLE_SET_OK:
		single_set_ok();
		break;
	case ALL_SET_OK:
		all_set_ok();
		break;
	case PID_SET_OK:
		pid_set_ok();
		break;
	case PID_PAGE_ENTER:
		pid_page_num = 1;
		update_pid_page(pid_page_num);
		break;
	case PID_PAGE_UP:
		if(--pid_page_num < 1)
			pid_page_num = MAX_IQR_QUANTITY;
		update_pid_page(pid_page_num);
		break;
	case PID_PAGE_DOWN:
		if(++pid_page_num > MAX_IQR_QUANTITY)
			pid_page_num = 1;
		update_pid_page(pid_page_num); 
		break;
	case CURVE_ZOOM_OUT:
		curve_time_level--;
		update_curve_page();
		break;
	case CURVE_ZOOM_IN:
		curve_time_level++;
		update_curve_page();
		break;
	case CURVE_PAGE_UP:
		curve_page_num--;
		update_curve_page();
		break;
	case CURVE_PAGE_DOWN:
		curve_page_num++;
		update_curve_page();
		break;
	case CLEAR_ALARM_MSG:
		clear_all_alarm_msg();
		break;
	case ALARM_PAGE_UP:
		if (--alarm_page_num <= 0)
			alarm_page_num = 1;
		update_alarm_page(alarm_page_num);
		break;
	case ALARM_PAGE_DOWN:
		if (++alarm_page_num >= max_alarm_page_num)
			alarm_page_num = max_alarm_page_num;
		update_alarm_page(alarm_page_num);
		break;
	case SINGLE_SET_BACK:
		single_set_back();
		break;
	case ALL_SET_BACK:
		all_set_back();
		break;
	case ALARM_PAGE_ENTER:
		alarm_page_num = 1;
		update_alarm_page(alarm_page_num);
		break;
	case CURVE_PAGE_ENTER:
		update_curve_page();
		break;
	case CURVE_PAGE_BACK:
		CLOSE_CURVE;
		update_run_temp_flag = 0;
		break;
	case MENU_PAGE_ENTER:
		in_main_page = 0;
		break;
	case TIME_CTRL_ENTER:
		module_num = 1;
		update_time_ctrl_page();
		break;
	case TIME_CTRL_STOP:
		stop_time_ctrl(module_num);
		break;
	case TIME_CTRL_START:
		start_time_ctrl(module_num);
		break;
	case TIME_CTRL_SAVEDAT:
		save_time_ctrl_data();
		break;
	case TIME_CTRL_PAGEUP:
		module_num -= 1;
		update_time_ctrl_page();
		break;
	case TIME_CTRL_PAGEDOWN:
		module_num += 1;
		update_time_ctrl_page();
		break;
	case IQR1_TEST:
		time_ctrl_test(module_num, IQR1_TEST);
		break;
	case IQR2_TEST:
		time_ctrl_test(module_num, IQR2_TEST);
		break;
	case IQR3_TEST:
		time_ctrl_test(module_num, IQR3_TEST);
		break;
	case IQR4_TEST:
		time_ctrl_test(module_num, IQR4_TEST);
		break;
	case IQR5_TEST:
		time_ctrl_test(module_num, IQR5_TEST);
		break;
	case IQR6_TEST:
		time_ctrl_test(module_num, IQR6_TEST);
		break;
	case IQR7_TEST:
		time_ctrl_test(module_num, IQR7_TEST);
		break;
	case IQR8_TEST:
		time_ctrl_test(module_num, IQR8_TEST);
		break;
	default:
		break;
	}
}

void single_set_ok(void)
{
	if (set_temp_buff[set_num] != set_temp[set_num])
	{
		set_temp[set_num] = set_temp_buff[set_num];
		single_set(TEMP, set_temp[set_num]);
		// 		eeprom_write(SINGLE_SETTEMP_EEADDR + (set_num * 2), set_temp[set_num]);
		// 		eeprom_write(SINGLE_SETTEMP_EEADDR + (set_num * 2) + 1, set_temp[set_num] >> 8);
	}

	if (switch_sensor_buff[set_num] != switch_sensor[set_num])
	{
		switch_sensor[set_num] = switch_sensor_buff[set_num];
		single_set(SWITCH_SENSOR, switch_sensor[set_num]);
		//		eeprom_write(SINGLE_SWSENSOR_EEADDR+set_num, switch_sensor[set_num]);	//开启默认关闭 不用写eeprom
	}

	// 	if (sensor_type_buff[set_num] != sensor_type[set_num])			//单独设定传感器类型功能取消
	// 	{
	// 		sensor_type[set_num] = sensor_type_buff[set_num];
	// 		ctrl_command = SENSOR_TYPE;
	// //		eeprom_write(SINGLE_SENSORTYPE_EEADDR + set_num, sensor_type[set_num]);
	// 	}

	if (follow_sta_buff[set_num] != follow_sta[set_num])
	{
		follow_sta[set_num] = follow_sta_buff[set_num];
		single_set(SET_FOLLOW, follow_sta[set_num]);
	}

	if (set_name_buff != set_name[set_num])
	{
		set_name[set_num] = set_name_buff;
		//设置名字   发送给主控板
		// 		eeprom_write(SET_NAME_EEADDR + (set_num * 4), set_name[set_num]);
		// 		eeprom_write(SET_NAME_EEADDR + (set_num * 4) + 1, set_name[set_num] >> 8);
		// 		eeprom_write(SET_NAME_EEADDR + (set_num * 4) + 2, set_name[set_num] >> 16);
		// 		eeprom_write(SET_NAME_EEADDR + (set_num * 4) + 3, set_name[set_num] >> 24);
	}

	update_main_page();
	in_main_page = 1;
	update_run_temp_flag = 0;
}

void all_set_ok(void)
{
	uint8_t i = 0;

	all_temp = all_temp_buff;

	for (i = 0; i < MAX_IQR_QUANTITY; i++)
	{
		set_temp[i] = all_temp;
		set_temp_buff[i] = all_temp;
		// 		eeprom_write(SINGLE_SETTEMP_EEADDR + (i * 2), set_temp[i]);
		// 		eeprom_write(SINGLE_SETTEMP_EEADDR + (i * 2) + 1, set_temp[i] >> 8);
	}

	// 	if (temp_unit_buff != temp_unit)
	// 	{
	temp_unit = temp_unit_buff;
	// 		send_variables(TEMP_UINT_ADDR, (CELSIUS + temp_unit * FAHRENHEIT));
	//
	// 		//发送给控制板卡  保存数据
	// 		/*		eeprom_write(TEMP_UNIT_EEADDR, temp_unit);*/
	// 	}

	preheat_time = preheat_time_buff;

	all_sensor_type = all_sensor_type_buff;

	for (i = 0; i < MAX_IQR_QUANTITY; i++)
	{
		sensor_type[i] = all_sensor_type;
		sensor_type_buff[i] = all_sensor_type;
		//			eeprom_write(SINGLE_SENSORTYPE_EEADDR + i, sensor_type[i]);
	}
	//	send_variables(ALL_SENSOR_TYPE_ADDR, TYPE_J + (all_sensor_type * TYPE_K));

	update_main_page();

	all_set(TEMP, all_temp);
	all_set(PREHEAT_TIME, preheat_time);
	all_set(SENSOR_TYPE, all_sensor_type);

	// 	eeprom_write(ALL_SETTEMP_EEADDR, all_temp);
	// 	eeprom_write(ALL_SETTEMP_EEADDR + 1, all_temp >> 8);

	// 	if (preheat_time_buff != preheat_time)
	// 	{
	// 		preheat_time = preheat_time_buff;
	//
	// 		all_set(PREHEAT_TIME, preheat_time);
	//
	// // 		eeprom_write(PREHEAT_TIME_EEADDR, preheat_time);
	// 	}
	//
	// 	if (all_sensor_type_buff != all_sensor_type)
	// 	{
	// 		all_sensor_type = all_sensor_type_buff;
	//
	// 		all_set(SENSOR_TYPE, all_sensor_type);
	// //		eeprom_write(ALL_SENSORTYPE_EEADDR, all_sensor_type);
	//
	// 		for (i = 0; i < MAX_IQR_QUANTITY; i++)
	// 		{
	// 			sensor_type[i] = all_sensor_type;
	// 			sensor_type_buff[i] = all_sensor_type;
	// //			eeprom_write(SINGLE_SENSORTYPE_EEADDR + i, sensor_type[i]);
	// 		}
	// 		send_variables(ALL_SENSOR_TYPE_ADDR, TYPE_J + (all_sensor_type * TYPE_K));
	// 	}
	//
	in_main_page = 1;
}

void pid_set_ok(void)
{
	p_value[set_pid_channel - 1] = p_value_buff;
	i_value[set_pid_channel - 1] = i_value_buff;
	d_value[set_pid_channel - 1] = d_value_buff;

	// 	eeprom_write(PID_P_EEADDR + ((set_pid_channel - 1) * 2), p_value[set_pid_channel - 1]);
	// 	eeprom_write(PID_P_EEADDR + ((set_pid_channel - 1) * 2) + 1, p_value[set_pid_channel - 1] >> 8);
	//
	// 	eeprom_write(PID_I_EEADDR + ((set_pid_channel - 1) * 2), i_value[set_pid_channel - 1]);
	// 	eeprom_write(PID_I_EEADDR + ((set_pid_channel - 1) * 2) + 1, i_value[set_pid_channel - 1] >> 8);
	//
	// 	eeprom_write(PID_D_EEADDR + ((set_pid_channel - 1) * 2), d_value[set_pid_channel - 1]);
	// 	eeprom_write(PID_D_EEADDR + ((set_pid_channel - 1) * 2) + 1, d_value[set_pid_channel - 1] >> 8);

	send_variables(PID_P_ADDR, p_value[set_pid_channel - 1]);
	send_variables(PID_I_ADDR, i_value[set_pid_channel - 1]);
	send_variables(PID_D_ADDR, d_value[set_pid_channel - 1]);

	set_pid();
}

/*
**设置PID参数，通过串口1发送到控制办卡
*/
void set_pid(void)
{
	uint16_t crc = 0;
	uint8_t addr = 0;

	/*计算地址，前四位代表示板卡号，后四位表示*/
	addr = (set_pid_channel - 1) / 4 + 1;
	addr = (addr << 4) + ((set_pid_channel - 1) % 4) + 1;

	usart1_tx_buff[0] = 0xA5;
	usart1_tx_buff[1] = 0x5A;
	usart1_tx_buff[2] = 0x08;
	usart1_tx_buff[3] = PID;
	usart1_tx_buff[4] = addr;

	usart1_tx_buff[5] = (uint8_t)(p_value_buff >> 8);
	usart1_tx_buff[6] = (uint8_t)(p_value_buff);

	usart1_tx_buff[7] = (uint8_t)(i_value_buff >> 8);
	usart1_tx_buff[8] = (uint8_t)(i_value_buff);

	usart1_tx_buff[9] = (uint8_t)(d_value_buff >> 8);
	usart1_tx_buff[10] = (uint8_t)(d_value_buff);

	crc = crc_check(usart1_tx_buff, 13);

	usart1_tx_buff[11] = crc & 0x00FF;
	usart1_tx_buff[12] = crc >> 8;

	ctrl_command = PID;

	while (usart1_tx_overtime_mask != 1)
		;
	usart1_tx_overtime_mask = 0;
	usart1_tx_timecnt = 0;

	usart1_send_str(usart1_tx_buff, 13);

	ctrl_command = READ_DATA_ALL;
}

void single_set_back(void)
{
	if (set_temp_buff[set_num] != set_temp[set_num])
	{
		set_temp_buff[set_num] = set_temp[set_num];
	}

	if (switch_sensor_buff[set_num] != switch_sensor[set_num])
	{
		switch_sensor_buff[set_num] = switch_sensor[set_num];
	}

	if (sensor_type_buff[set_num] != sensor_type[set_num])
	{
		sensor_type_buff[set_num] = sensor_type[set_num];
	}

	in_main_page = 1;
	update_run_temp_flag = 0;
}

void all_set_back(void)
{
	if (all_temp_buff != all_temp)
	{
		all_temp_buff = all_temp;
		send_variables(ALL_SET_TEMP, all_temp);
	}

	if (preheat_time_buff != preheat_time)
	{
		preheat_time_buff = preheat_time;
		send_variables(SET_PREHEAT_TIME, preheat_time);
	}

	if (all_sensor_type_buff != all_sensor_type)
	{
		all_sensor_type_buff = all_sensor_type;
		send_variables(ALL_SENSOR_TYPE_ADDR, TYPE_J + (all_sensor_type * TYPE_K));
	}

	if (temp_unit_buff != temp_unit)
	{
		temp_unit_buff = temp_unit;
		send_variables(TEMP_UINT_ADDR, (CELSIUS + temp_unit * FAHRENHEIT));
	}

	in_main_page = 1;
}

void switch_all_sensor(uint16_t sta)
{
	uint8_t i = 0;
	uint16_t crc = 0;
	uint8_t slave_num = 1;

	ctrl_command = 0; //不是READ_DATA_ALL 就ok

	usart1_tx_buff[0] = 0xA5;
	usart1_tx_buff[1] = 0x5A;
	usart1_tx_buff[2] = 0x04;
	usart1_tx_buff[3] = 0x06;
	usart1_tx_buff[5] = 0x00;
	usart1_tx_buff[6] = sta;

	pre_system_sta = sta;

	for (i = 0; i < MAX_IQR_QUANTITY; i++)
	{
		switch_sensor[i] = sta;
		switch_sensor_buff[i] = sta;
		// 		eeprom_write(SINGLE_SWSENSOR_EEADDR+i, switch_sensor[i]);
	}

	for (; slave_num <= TEMP_CTRL_BOARD_QUANTITY; slave_num++)
	{
		usart1_tx_buff[4] = slave_num << 4;

		crc = crc_check(usart1_tx_buff, 9);

		usart1_tx_buff[7] = crc & 0x00FF;
		usart1_tx_buff[8] = crc >> 8;

		while (usart1_tx_overtime_mask != 1)
			;
		usart1_tx_overtime_mask = 0;
		usart1_tx_timecnt = 0;

		usart1_send_str(usart1_tx_buff, 9);
	}

	ctrl_command = READ_DATA_ALL;
}

void get_set_name(void)
{
	set_name_buff = usart0_rx_buff[7];
	set_name_buff = (set_name_buff << 8) | usart0_rx_buff[8];
	set_name_buff = (set_name_buff << 8) | usart0_rx_buff[9];
	set_name_buff = (set_name_buff << 8) | usart0_rx_buff[10];
}

void single_set(uint8_t command, uint16_t value)
{
	uint16_t crc = 0;
	uint8_t addr = 0;

	ctrl_command = 0; // 不是READ_DATA_ALL 就OK

	if (temp_unit_buff == 1)
	{
		value = ((value - 32) * 10 / 18);
	}

	addr = (set_num / 4) + 1;
	addr = (addr << 4) | ((set_num % 4) + 1);

	usart1_tx_buff[0] = 0xA5;
	usart1_tx_buff[1] = 0x5A;
	usart1_tx_buff[2] = 0x04;
	usart1_tx_buff[3] = command;
	usart1_tx_buff[4] = addr;
	usart1_tx_buff[5] = (value >> 8);
	usart1_tx_buff[6] = value;

	crc = crc_check(usart1_tx_buff, 9);

	usart1_tx_buff[7] = crc & 0x00FF;
	usart1_tx_buff[8] = crc >> 8;

	while (usart1_tx_overtime_mask != 1)
		;
	usart1_tx_overtime_mask = 0;
	usart1_tx_timecnt = 0;

	usart1_send_str(usart1_tx_buff, 9);

	ctrl_command = READ_DATA_ALL;
}

void all_set(uint8_t command, uint16_t value)
{
	uint16_t crc = 0;
	uint8_t slave_num = 1;

	ctrl_command = 0; //不是READ_DATA_ALL 就ok

	if (command == TEMP)
	{
		if (temp_unit_buff == 1)
		{
			value = ((value - 32) * 10 / 18);
		}
	}

	usart1_tx_buff[0] = 0xA5;
	usart1_tx_buff[1] = 0x5A;
	usart1_tx_buff[2] = 0x04;
	usart1_tx_buff[3] = command;
	usart1_tx_buff[5] = (value >> 8);
	usart1_tx_buff[6] = value;

	for (; slave_num <= TEMP_CTRL_BOARD_QUANTITY; slave_num++)
	{
		usart1_tx_buff[4] = slave_num << 4;

		crc = crc_check(usart1_tx_buff, 9);

		usart1_tx_buff[7] = crc & 0x00FF;
		usart1_tx_buff[8] = crc >> 8;

		while (usart1_tx_overtime_mask != 1)
			;
		usart1_tx_overtime_mask = 0;
		usart1_tx_timecnt = 0;

		usart1_send_str(usart1_tx_buff, 9);
	}

	ctrl_command = READ_DATA_ALL;
}

void update_single_set_page(void)
{
	in_main_page = 0;

	update_run_temp_flag = 1;
	run_temp_page = 1;

	send_variables(SINGLE_NUM_ADDR, set_num + 1);															  //序号显示
	send_variables(SINGLE_RUNTEMP_ADDR, run_temp[set_num] + temp_unit * ((run_temp[set_num] * 8 / 10) + 32)); //运行温度显示
	send_variables(SINGLE_SETTEMP_ADDR, set_temp[set_num]);													  //设定温度显示
	send_variables(SINGLE_SET_TEMP, set_temp[set_num]);														  //温度设定输入框显示
	send_variables(SINGLE_SET_SWITCH, switch_sensor[set_num]);												  //传感器开启/关闭状态
	send_variables(SINGLE_SENSORTYPE_ADDR, TYPE_J + (sensor_type[set_num] * TYPE_K));						  //DANDUPAGE传感器类型选择显示

	send_name(SINGLE_NAME_ADDR, set_name[set_num]);

	send_name(SINGLE_SET_NAME, set_name[set_num]);

	set_name_buff = set_name[set_num];
}

void update_pid_page(uint8_t channel)
{
	set_pid_channel = channel;

	send_variables(PID_CHANNEL, channel);

	send_variables(PID_P_ADDR, p_value[channel - 1]);
	send_variables(PID_I_ADDR, i_value[channel - 1]);
	send_variables(PID_D_ADDR, d_value[channel - 1]);

	send_variables(PID_P, p_value[channel - 1]);
	send_variables(PID_I, i_value[channel - 1]);
	send_variables(PID_D, d_value[channel - 1]);

	p_value_buff = p_value[channel - 1];
	i_value_buff = i_value[channel - 1];
	d_value_buff = d_value[channel - 1];
}

void update_curve_page(void)
{
	uint8_t i = 0;
	uint8_t count = 0;
	uint16_t time_line[10] = {2, 4, 6, 8, 10, 12, 14, 16, 18, 20};

	update_run_temp_flag = 1;
	run_temp_page = 0;

	if (curve_time_level >= 4)
	{
		curve_time_level = 3;
	}
	else if (curve_time_level < 0)
	{
		curve_time_level = 0;
	}

	if (curve_page_num >= MAX_IQR_QUANTITY)
	{
		curve_page_num = 0;
	}
	else if (curve_page_num < 0)
	{
		curve_page_num = MAX_IQR_QUANTITY - 1;
	}

	count = 10 * (curve_time_level + 1);

	CLOSE_CURVE;
	clear_curve_buff(CHANNEL0_BUFF);

	for (i = 0; i < 10; i++)
	{
		time_line[i] *= (curve_time_level + 1);
		send_variables(CURVE_TIMELINE_ADDR + (i * 2), time_line[i]);
	}

	send_variables(CURVE_PAGE_NUM_ADDR, curve_page_num + 1);
	send_variables(CURVE_PAGE_RUNTEMP_ADDR, run_temp[curve_page_num] +
												temp_unit * (run_temp[curve_page_num] * 8 / 10 + 32)); //运行温度
	send_variables(CURVE_PAGE_SETTEMP_ADDR, set_temp[curve_page_num]);								   //设定温度
	send_variables(CURVE_PAGE_OUTRATE_ADDR, output_rate[curve_page_num]);
	send_variables(CURVE_PAGE_SENSORTYPE_ADDR, TYPE_J + (sensor_type[curve_page_num] * TYPE_K));
	send_name(CURVE_PAGE_NAME_ADDR, set_name[curve_page_num]);

	OPEN_CURVE(count);
}

/*更新射胶时间控制界面*/
void update_time_ctrl_page(void)
{
	uint8_t j, k;
	uint8_t addr_offset = 0x00;

	if (module_num < 1)
	{
		module_num = 1;
	}
	else if (module_num > 4)
	{
		module_num = 4;
	}
	else
	{
		send_variables(MODULE_NUM_ADDR, module_num);
		send_variables(MODULE_STATUS_ADDR, module_status[module_num - 1]);

		for (j = 0; j < 8; j++)
		{
			for (k = 0; k < 4; k++)
			{
				send_variables(IQR1_T1 + addr_offset, time_ctrl_value[module_num - 1][j][k]);
				addr_offset += 0x02;
			}
		}
	}
}

/*按下保存键 保存输入的数据  不按保存直接返回的话  则不保存*/
void save_time_ctrl_data(void)
{
	uint8_t i, j;

	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 4; j++)
		{
			time_ctrl_value[module_num - 1][i][j] = time_ctrl_value_buff[module_num - 1][i][j];
			eeprom_write_word((uint16_t *)(TIME_CTRL_VALUE_EEADDR + ((module_num - 1) * 32 + i * 4 + j) * 2), time_ctrl_value[module_num - 1][i][j]);
		}
	}
}

/*向时间控制子模块发送开始命令 并发送每个通道的 T1—T4 数据*/
void start_time_ctrl(uint8_t slave_num)
{
	uint8_t i, j;
	uint8_t cnt = 0; //计数第几个数据
	uint16_t crc = 0;

	usart2_tx_buff[0] = 0xA5;
	usart2_tx_buff[1] = 0x5A;
	usart2_tx_buff[2] = 0x42;
	usart2_tx_buff[3] = TIME_CTRL_START;
	usart2_tx_buff[4] = slave_num - 1;

	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 4; j++)
		{
			usart2_tx_buff[cnt + 5] = time_ctrl_value[slave_num - 1][i][j] >> 8;
			usart2_tx_buff[cnt + 6] = time_ctrl_value[slave_num - 1][i][j];
			cnt += 2;
		}
	}

	crc = crc_check(usart2_tx_buff, 71);

	usart2_tx_buff[69] = crc & 0x00FF;
	usart2_tx_buff[70] = crc >> 8;

	usart2_send_str(usart2_tx_buff, 71);

	module_status[slave_num - 1] = 1;
	send_variables(MODULE_STATUS_ADDR, module_status[slave_num - 1]);
	PORTA &= ~(1 << (7 - slave_num));
}

/*射胶阀时间控制通道测试函数*/
void time_ctrl_test(uint8_t slave_num, uint8_t command)
{
	uint8_t i;
	uint8_t cnt = 0; //计数第几个数据
	uint16_t crc = 0;

	usart2_tx_buff[0] = 0xA5;
	usart2_tx_buff[1] = 0x5A;
	usart2_tx_buff[2] = 0x0A;
	usart2_tx_buff[3] = command;
	usart2_tx_buff[4] = slave_num - 1;

	for (i = 0; i < 4; i++)
	{
		usart2_tx_buff[cnt + 5] = time_ctrl_value[slave_num - 1][command - 0x21][i] >> 8;
		usart2_tx_buff[cnt + 6] = time_ctrl_value[slave_num - 1][command - 0x21][i];
		cnt += 2;
	}

	crc = crc_check(usart2_tx_buff, 15);

	usart2_tx_buff[13] = crc & 0x00FF;
	usart2_tx_buff[14] = crc >> 8;

	usart2_send_str(usart2_tx_buff, 15);

	// module_status[slave_num - 1] = 1;
	// send_variables(MODULE_STATUS_ADDR, module_status[slave_num - 1]);
	// PORTA &= ~(1 << (7 - slave_num));
}

/*向时间控制子模块发送停止命令 不需要发送每个通道的 T1—T4 数据*/
void stop_time_ctrl(uint8_t slave_num)
{
	uint16_t crc = 0;

	usart2_tx_buff[0] = 0xA5;
	usart2_tx_buff[1] = 0x5A;
	usart2_tx_buff[2] = 0x02;
	usart2_tx_buff[3] = TIME_CTRL_STOP;
	usart2_tx_buff[4] = slave_num - 1;

	crc = crc_check(usart2_tx_buff, 7);

	usart2_tx_buff[5] = crc & 0x00FF;
	usart2_tx_buff[6] = crc >> 8;

	usart2_send_str(usart2_tx_buff, 7);

	module_status[slave_num - 1] = 0;
	send_variables(MODULE_STATUS_ADDR, module_status[slave_num - 1]);
	PORTA |= (1 << (7 - slave_num));
}

void update_alarm_page(uint8_t page_num)
{
	uint8_t i = 0;
	uint8_t alarm_msg_num = 0;

	for (i = 0; i < 7; i++)
	{
		alarm_msg_num = alarm_cnt - i - (page_num - 1) * 7;
		send_variables(ALARM_PAGE_NUM1_ADDR + (i * 2), i + ((page_num - 1) * 7) + 1);

		if (alarm_cnt > i + (page_num - 1) * 7)
		{
			send_alarm_msg(alarm_history[alarm_msg_num - 1].alarm_type, i, alarm_history[alarm_msg_num - 1].alarm_device_num);
		}
		else
		{
			clear_alarm_msg(i);
		}
	}

	/*
	for (i = 0; i < MAX_IQR_QUANTITY; i++)
	{
		if (sensor_sta[i] >= 3)
		{
			sensor_num = i;
			send_alarm_msg(sensor_sta[i], alarm_msg_num, sensor_num);
			alarm_msg_num++;
		}
	}

	for (; alarm_msg_num < 7; alarm_msg_num++)
	{
		clear_alarm_msg(alarm_msg_num);
	}
*/
}

void switch_language(void)
{
	if (pre_language == 1)
	{
		change_page(3); //暂定英文页面为13
	}
	else
	{
		change_page(1);
	}

	eeprom_write_byte((uint8_t *)PRE_LANGUAGE_EEADDR, pre_language);
}

void change_page(uint8_t page_num)
{
	uint16_t crc = 0;

	usart0_tx_buff[0] = 0xA5;
	usart0_tx_buff[1] = 0x5A;
	usart0_tx_buff[2] = 0x04; //4.3寸屏为4  5寸屏为6
	usart0_tx_buff[3] = 0x80;
	usart0_tx_buff[4] = 0x03;
	usart0_tx_buff[5] = 0x00;
	usart0_tx_buff[6] = page_num;

	crc = crc_check(usart0_tx_buff, 9);

	usart0_tx_buff[7] = crc & 0x00FF;
	usart0_tx_buff[8] = crc >> 8;

	usart0_send_str(usart0_tx_buff, 9);
}

/**
* @brief  指定地址下发变量到dgus
* @param  addr变量地址  data变量值
* @retval None
*/
void send_variables(uint16_t addr, uint16_t data)
{
	uint16_t crc = 0;
	usart0_tx_buff[0] = 0xA5;
	usart0_tx_buff[1] = 0x5A;
	usart0_tx_buff[2] = 0x05;
	usart0_tx_buff[3] = 0x82;
	usart0_tx_buff[4] = addr >> 8;
	usart0_tx_buff[5] = addr & 0x00FF;
	usart0_tx_buff[6] = data >> 8;
	usart0_tx_buff[7] = data & 0x00FF;

	crc = crc_check(usart0_tx_buff, 10);

	usart0_tx_buff[8] = crc & 0x00FF;
	usart0_tx_buff[9] = crc >> 8;

	usart0_send_str(usart0_tx_buff, 10);
}

void send_name(uint16_t addr, uint32_t name)
{
	uint16_t crc = 0;

	usart0_tx_buff[0] = 0xA5;
	usart0_tx_buff[1] = 0x5A;
	usart0_tx_buff[2] = 0x07;
	usart0_tx_buff[3] = 0x82;
	usart0_tx_buff[4] = addr >> 8;
	usart0_tx_buff[5] = addr & 0x00FF;
	usart0_tx_buff[6] = name >> 24;
	usart0_tx_buff[7] = name >> 16;
	usart0_tx_buff[8] = name >> 8;
	usart0_tx_buff[9] = name;

	crc = crc_check(usart0_tx_buff, 12);

	usart0_tx_buff[10] = crc & 0x00FF;
	usart0_tx_buff[11] = crc >> 8;

	usart0_send_str(usart0_tx_buff, 12);
}

void send_to_channel(uint8_t ch, uint16_t value)
{
	uint16_t crc = 0;

	usart0_tx_buff[0] = 0xA5;
	usart0_tx_buff[1] = 0x5A;
	usart0_tx_buff[2] = 0x04;
	usart0_tx_buff[3] = 0x84;
	usart0_tx_buff[4] = ch;
	usart0_tx_buff[5] = value >> 8;
	usart0_tx_buff[6] = value;

	crc = crc_check(usart0_tx_buff, 9);

	usart0_tx_buff[7] = crc & 0x00FF;
	usart0_tx_buff[8] = crc >> 8;

	usart0_send_str(usart0_tx_buff, 9);
}

void send_alarm_msg(uint8_t type, uint8_t msg_num, uint16_t sen_num)
{
	uint8_t i = 0;
	uint16_t crc = 0;
	uint16_t sensor_num = 0;

	usart0_tx_buff[0] = 0xA5;
	usart0_tx_buff[1] = 0x5A;
	usart0_tx_buff[2] = 0x13;
	usart0_tx_buff[3] = 0x82;
	usart0_tx_buff[4] = (ALAEM_MSG1_ADDR + (msg_num * 16)) >> 8;
	usart0_tx_buff[5] = (ALAEM_MSG1_ADDR + (msg_num * 16)) & 0x00FF;

	for (i = 0; i < 16; i++)
	{
		usart0_tx_buff[i + 6] = alarm_msg[type][i];
	}

	crc = crc_check(usart0_tx_buff, 24);

	usart0_tx_buff[22] = crc & 0x00FF;
	usart0_tx_buff[23] = crc >> 8;

	usart0_send_str(usart0_tx_buff, 24);

	if (sen_num <= 8)
	{
		sensor_num = 0x2300 | (sen_num + 49);
	}
	else
	{
		sensor_num = 0x2300 | (sen_num + 56);
	}

	send_variables(ALARM_DEVICE_NUM1_ADDR + (msg_num * 2), sensor_num);
}

/* 依次读取三块主控板设定数据 */
void read_setting_data_all(void)
{
	uint8_t i, j; //i:板号   j：通道号

	for (i = 0; i < TEMP_CTRL_BOARD_QUANTITY; i++)
	{
		ctrl_board_sta[i] = read_setting_data(i+1);

		if (ctrl_board_sta[i] == 0)
		{
			for (j = 0; j < 4; j++)
			{
				sensor_sta[i * 4 + j] = 3;
			}
		}
	}
}

/*
*功能：读取一块主控板设定数据
*返回数据：0:断线 1:连接（收到数据）
*/
uint8_t read_setting_data(uint8_t addr)
{
	uint8_t board_sta = 0;
	uint16_t crc = 0;
	uint8_t i;
	read_setting_data_mask = 0;
	read_setting_data_cnt = 0;

	for (i = 1; i < 5; i++)
	{
		usart1_tx_buff[0] = 0xA5;
		usart1_tx_buff[1] = 0x5A;
		usart1_tx_buff[2] = 0x02;
		usart1_tx_buff[3] = READ_SETTING_DATA;
		usart1_tx_buff[4] = (addr << 4) + i;

		crc = crc_check(usart1_tx_buff, 7);

		usart1_tx_buff[5] = crc & 0x00FF;
		usart1_tx_buff[6] = crc >> 8;

		usart1_send_str(usart1_tx_buff, 7);

		while (read_setting_data_mask != 1)
		{
			if (usart1_rx_end)
			{
				usart1_deal();
				usart1_rx_end = 0;

				if (usart1_rx_buff[4] == ((addr << 4) + i))
				{
					board_sta = 1;
				}
			}
		}
		read_setting_data_mask = 0;
		read_setting_data_cnt = 0;
	}
	return board_sta;
}
void get_setting_data(uint8_t addr)
{
	uint8_t i;
	uint8_t iqr_num = ((addr >> 4) - 1) * 4 + (addr & 0x0F) - 1;

	set_name[iqr_num] = usart1_rx_buff[5];
	set_name[iqr_num] = (set_name[iqr_num] << 8) | usart1_rx_buff[6];
	set_name[iqr_num] = (set_name[iqr_num] << 8) | usart1_rx_buff[7];
	set_name[iqr_num] = (set_name[iqr_num] << 8) | usart1_rx_buff[8];

	set_temp[iqr_num] = usart1_rx_buff[9];
	set_temp[iqr_num] = (set_temp[iqr_num] << 8) | usart1_rx_buff[10];
	set_temp_buff[iqr_num] = set_temp[iqr_num];

	all_sensor_type = usart1_rx_buff[12];
	all_sensor_type_buff = all_sensor_type;

	for (i = 0; i < MAX_IQR_QUANTITY; i++)
	{
		sensor_type[i] = all_sensor_type;
	}

	follow_sta[iqr_num] = usart1_rx_buff[14];
	follow_sta_buff[iqr_num] = follow_sta[iqr_num];

	if (usart1_rx_buff[16] <= 2) //通道状态： 0：关闭  1：开启  2:跟随 3~n：告警类型
	{
		switch_sensor[iqr_num] = usart1_rx_buff[16] & 0x01; //0：关闭  1：开启 （跟随归属到开启）
		switch_sensor_buff[iqr_num] = switch_sensor[iqr_num];

		sensor_sta[iqr_num] = usart1_rx_buff[16];
	}
	else
	{
		sensor_sta[iqr_num] = usart1_rx_buff[16];
		switch_sensor[iqr_num] = 0;
		switch_sensor_buff[iqr_num] = 0;
	}

	p_value[iqr_num] = usart1_rx_buff[17];
	p_value[iqr_num] = (p_value[iqr_num] << 8) | usart1_rx_buff[18];

	i_value[iqr_num] = usart1_rx_buff[19];
	i_value[iqr_num] = (i_value[iqr_num] << 8) | usart1_rx_buff[20];

	d_value[iqr_num] = usart1_rx_buff[21];
	d_value[iqr_num] = (d_value[iqr_num] << 8) | usart1_rx_buff[22];

	//
	// 	temp_unit = usart1_rx_buff[19];
	// 	temp_unit_buff = temp_unit;
	//
	// 	all_temp = usart1_rx_buff[20];
	// 	all_temp = (all_temp << 8) | usart1_rx_buff[21];
	// 	all_temp_buff = all_temp;
	//
	// 	preheat_time = usart1_rx_buff[22];
	// 	preheat_time_buff = preheat_time;

	//	pre_system_sta
}

void send_request_all(uint8_t addr)
{
	uint16_t crc = 0;

	usart1_tx_buff[0] = 0xA5;
	usart1_tx_buff[1] = 0x5A;
	usart1_tx_buff[2] = 0x02;
	usart1_tx_buff[3] = 0x01;
	usart1_tx_buff[4] = (addr << 4);

	crc = crc_check(usart1_tx_buff, 7);

	usart1_tx_buff[5] = crc & 0x00FF;
	usart1_tx_buff[6] = crc >> 8;

	usart1_send_str(usart1_tx_buff, 7);
}

void get_data_all()
{
	uint8_t slave_num = 0;

	slave_num = (usart1_rx_buff[4] >> 4);

	output_rate[0 + ((slave_num - 1) * 4)] = usart1_rx_buff[5];
	output_rate[1 + ((slave_num - 1) * 4)] = usart1_rx_buff[6];
	output_rate[2 + ((slave_num - 1) * 4)] = usart1_rx_buff[7];
	output_rate[3 + ((slave_num - 1) * 4)] = usart1_rx_buff[8];

	sensor_sta[0 + ((slave_num - 1) * 4)] = usart1_rx_buff[9];
	sensor_sta[1 + ((slave_num - 1) * 4)] = usart1_rx_buff[10];
	sensor_sta[2 + ((slave_num - 1) * 4)] = usart1_rx_buff[11];
	sensor_sta[3 + ((slave_num - 1) * 4)] = usart1_rx_buff[12];

	run_temp[0 + ((slave_num - 1) * 4)] = usart1_rx_buff[13];
	run_temp[0 + ((slave_num - 1) * 4)] = (run_temp[0 + ((slave_num - 1) * 4)] << 8) | usart1_rx_buff[14];
	run_temp[1 + ((slave_num - 1) * 4)] = usart1_rx_buff[15];
	run_temp[1 + ((slave_num - 1) * 4)] = (run_temp[1 + ((slave_num - 1) * 4)] << 8) | usart1_rx_buff[16];
	run_temp[2 + ((slave_num - 1) * 4)] = usart1_rx_buff[17];
	run_temp[2 + ((slave_num - 1) * 4)] = (run_temp[2 + ((slave_num - 1) * 4)] << 8) | usart1_rx_buff[18];
	run_temp[3 + ((slave_num - 1) * 4)] = usart1_rx_buff[19];
	run_temp[3 + ((slave_num - 1) * 4)] = (run_temp[3 + ((slave_num - 1) * 4)] << 8) | usart1_rx_buff[20];

	sensor_sta[0 + ((slave_num - 1) * 4)] = usart1_rx_buff[21];
	sensor_sta[1 + ((slave_num - 1) * 4)] = usart1_rx_buff[22];
	sensor_sta[2 + ((slave_num - 1) * 4)] = usart1_rx_buff[23];
	sensor_sta[3 + ((slave_num - 1) * 4)] = usart1_rx_buff[24];

	update_main_page();
}

void clear_alarm_msg(uint8_t msg_num)
{
	uint8_t i = 0;
	uint16_t crc = 0;

	usart0_tx_buff[0] = 0xA5;
	usart0_tx_buff[1] = 0x5A;
	usart0_tx_buff[2] = 0x13;
	usart0_tx_buff[3] = 0x82;
	usart0_tx_buff[4] = (ALAEM_MSG1_ADDR + (msg_num * 16)) >> 8;
	usart0_tx_buff[5] = (ALAEM_MSG1_ADDR + (msg_num * 16)) & 0x00FF;

	for (i = 0; i < 16; i++)
	{
		usart0_tx_buff[i + 6] = 0x00;
	}

	crc = crc_check(usart0_tx_buff, 24);

	usart0_tx_buff[22] = crc & 0x00FF;
	usart0_tx_buff[23] = crc >> 8;

	usart0_send_str(usart0_tx_buff, 24);

	send_variables(ALARM_DEVICE_NUM1_ADDR + (msg_num * 2), 0x0000);
}

/*清除所有告警清单中的信息*/
void clear_all_alarm_msg(void)
{
	uint8_t i;

	alarm_cnt = 0;

	for (i = 0; i < 7; i++)
	{
		clear_alarm_msg(i);
	}

	update_alarm_page(1);
	eeprom_write_byte((uint8_t *)ALARM_CNT_EEADDR, alarm_cnt);
}

void clear_curve_buff(uint8_t channel)
{
	uint16_t crc = 0;

	usart0_tx_buff[0] = 0xA5;
	usart0_tx_buff[1] = 0x5A;
	usart0_tx_buff[2] = 0x03;
	usart0_tx_buff[3] = 0x80;
	usart0_tx_buff[4] = 0xEB;
	usart0_tx_buff[5] = channel;

	crc = crc_check(usart0_tx_buff, 8);

	usart0_tx_buff[6] = crc & 0x00FF;
	usart0_tx_buff[7] = crc >> 8;

	usart0_send_str(usart0_tx_buff, 8);
}

void alarm_monitor(void)
{
	uint8_t i, j;
	uint16_t alarm_sta_mask = 0;

	for (i = 0; i < MAX_IQR_QUANTITY; i++)
	{
		if (sensor_sta[i] != pre_sensor_sta[i])
		{
			pre_sensor_sta[i] = sensor_sta[i];

			if (sensor_sta[i] >= 3)
			{
				ALARM_ON;

				if (alarm_cnt >= MAX_ALARM_HISTORY - 1)
				{
					alarm_cnt = MAX_ALARM_HISTORY - 1;
					for (j = 0; j < alarm_cnt; j++)
					{
						alarm_history[j] = alarm_history[j + 1];
					}
				}
				alarm_history[alarm_cnt].alarm_type = sensor_sta[i] - 3;
				alarm_history[alarm_cnt].alarm_device_num = i;

				eeprom_write_byte((uint8_t *)(ALARM_HISTORY_EEADDR + alarm_cnt * 2 + 1),
								  alarm_history[alarm_cnt].alarm_type);
				eeprom_write_byte((uint8_t *)(ALARM_HISTORY_EEADDR + alarm_cnt * 2),
								  alarm_history[alarm_cnt].alarm_device_num);
				eeprom_write_byte((uint8_t *)ALARM_CNT_EEADDR, ++alarm_cnt);
			}
		}

		if (sensor_sta[i] <= 2)
		{
			alarm_sta_mask |= 1 << i;
		}
	}

	/*如果无告警 则自动关闭报警器*/
	if (alarm_sta_mask == 0x0FFF)
	{
		ALARM_OFF;
	}
	alarm_sta_mask = 0;
}
