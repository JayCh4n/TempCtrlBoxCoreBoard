/*
 * dgus.c
 *
 * Created: 2018-04-04 09:46:43
 *  Author: chenlong
 */ 

#include "dgus.h"

uint16_t pre_iqr = 0;
uint8_t run_temp_page = 0;
uint8_t update_run_temp_flag = 0;
uint8_t in_main_page = 1;			//当前是否在主界面，刷新主页面数据标志位 0：否 1：是
uint8_t pre_language = 0;			//当前语言
uint8_t pre_main_page = 0;			//当前主页面页号码
uint16_t pre_system_sta = 0;		//当前系统状态
uint8_t set_num = 0;				//单独设定键传感器位号
uint8_t set_pid_channel = 0;
int8_t curve_page_num = 0;			//曲线界面号
int8_t curve_time_level = 0;		//曲线时间轴大小

uint8_t output_rate[12] = {0};		//获取
uint8_t sensor_sta[12] = {0};		//获取
int16_t run_temp[12] = {0};			//获取
uint8_t alarm_type[12]= {0};		//获取

int16_t set_temp[12] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};			//发送
uint16_t switch_sensor[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t sensor_type[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint32_t set_name[12] = {0x23310000, 0x23320000, 0x23330000,0x23340000, 0x23350000, 0x23360000,
						 0x23370000, 0x23380000, 0x23390000,0x23313000, 0x23313100, 0x23313200};
uint32_t set_name_buff = 0;

uint16_t p_value[12] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};				//发送
uint16_t i_value[12] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};				//发送
uint16_t d_value[12] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};				//发送

uint16_t t1[8] = {500, 500, 500, 500, 500, 500, 500, 500};
uint16_t t2[8] = {500, 500, 500, 500, 500, 500, 500, 500};
uint16_t t3[8] = {500, 500, 500, 500, 500, 500, 500, 500};
uint16_t t4[8] = {500, 500, 500, 500, 500, 500, 500, 500};

uint8_t first_start_iqr[8] = {0};
uint8_t start_iqr[8] = {0};

uint16_t preheat_time = 3;			//发送
uint16_t all_sensor_type = 0;		//发送	
uint16_t temp_unit = 0;				//				
uint16_t all_temp = 100;			//发送

uint8_t in_time_ctrl_page = 0;

uint8_t alarm_msg[4][16] ={{0xB8, 0xD0, 0xCE, 0xC2, 0xCF, 0xDF, 0xB6, 0xCF, 0xBF, 0xAA},		//感温线断开
						   {0xB8, 0xD0, 0xCE, 0xC2, 0xCF, 0xDF, 0xBD, 0xD3, 0xB7, 0xB4},		//感温线接反
						   {0xB3, 0xAC, 0xCE, 0xC2, 0xB1, 0xA8, 0xBE, 0xAF},					//超温报警
						   {0xC9, 0xFD, 0xCE, 0xC2, 0xB5, 0xBD, 0xB4, 0xEF, 0xC9, 0xE8, 0xB6, 0xA8, 0xCE, 0xC2, 0xB6, 0xC8}}; //升温到达设定温度

void update_main_page(void)
{
	uint8_t i = 0;

	for (i=0; i<6;i++)
	{
		send_variables(MAINPAGE_NUM1_ADDR+(i*2), (i+(pre_main_page*6)+1));								//主页面传感器序号显示
		send_variables(MAIN_OUTRATE1_ADDR+(i*2), output_rate[i+(pre_main_page*6)]);						//传感器输出比例
		send_variables(MAIN_RUNTEMP1_ADDR+(i*2), run_temp[i+(pre_main_page*6)] + 
												 temp_unit*(run_temp[i+(pre_main_page*6)]*8/10 + 32));	//运行温度
		send_variables(MAIN_SETTEMP1_ADDR+(i*2), set_temp[i+(pre_main_page*6)]);						//设定温度	
		send_variables(MAIN_SENSOR1_TYPE_ADDR+(i*2), TYPE_J+(sensor_type[i+(pre_main_page*6)])*TYPE_K);
		
		send_name(MAIN_NAME1_ADDR+(i*8), set_name[i+(pre_main_page*6)]);
	}
}

void key_action(uint16_t key_code)
{
	switch(key_code)
	{
		case SWITCH_LANGUAGE:		pre_language ^= 0x01; switch_language();					break;
		case CLEAR_ALARM_SOUND:		ALARM_OFF;													break;
		case MAIN_PAGE_UP:			pre_main_page = 0;											break;
		case MAIN_PAGE_DOWN:		pre_main_page = 1;											break;
		case MAIN_SENSOR1_SET:		set_num = 0 + (pre_main_page*6); update_single_set_page();	break;
		case MAIN_SENSOR2_SET:		set_num = 1 + (pre_main_page*6); update_single_set_page();	break;
		case MAIN_SENSOR3_SET:		set_num = 2 + (pre_main_page*6); update_single_set_page();	break;
		case MAIN_SENSOR4_SET:		set_num = 3 + (pre_main_page*6); update_single_set_page();	break;
		case MAIN_SENSOR5_SET:		set_num = 4 + (pre_main_page*6); update_single_set_page();	break;
		case MAIN_SENSOR6_SET:		set_num = 5 + (pre_main_page*6); update_single_set_page();	break;
		case SINGLE_SET_OK:			single_set_ok();											break;
		case ALL_SET_OK:			ctrl_command = ALL_SET_CMD;								    break;
		case PID_SET_OK:			pid_set_ok();												break;
		case CURVE_ZOOM_OUT:		curve_time_level--; update_curve_page();					break;
		case CURVE_ZOOM_IN:			curve_time_level++;	update_curve_page();					break;
		case CURVE_PAGE_UP:			curve_page_num--;	update_curve_page();					break;
		case CURVE_PAGE_DOWN:		curve_page_num++;	update_curve_page();					break;
		case CLEAR_ALARM_MSG:		clear_alarm_msg(15);										break;
		case ALARM_PAGE_UP:			update_alarm_page(1);										break;
		case ALARM_PAGE_DOWN:		update_alarm_page(2);										break;
		case SINGLE_SET_BACK:		single_set_back();											break;
		case ALL_SET_BACK:			all_set_back();												break;
		case PID_PAGE_ENTER:		update_pid_page(1);											break;
		case ALARM_PAGE_ENTER:		update_alarm_page(1);										break;
		case CURVE_PAGE_ENTER:		update_curve_page();										break;
		case CURVE_PAGE_BACK:		CLOSE_CURVE; update_run_temp_flag = 0;						break;
		case MENU_PAGE_ENTER:		in_main_page = 0; 											break;
		case TIME_CTRL_ENTER:		in_time_ctrl_page = 1; update_time_ctrl_page();				break;
		case TIME_CTRL_BACK:		in_time_ctrl_page = 0; 										break;
		case AUTO_CTRL_TIME:		first_start_iqr[pre_iqr] = 1; start_iqr[pre_iqr] = 1;		break;
		case STOP_CTRL_TIME:		start_iqr[pre_iqr] = 0;										break;
		case TIME_CTRL_OK:			get_time_ctrl_value();										break;
		default: break;	
	}
}

void single_set_ok(void)
{
	if(set_temp_buff[set_num] != set_temp[set_num])
	{
		set_temp[set_num] = set_temp_buff[set_num];
		ctrl_command = TEMP;
		eeprom_write(SINGLE_SETTEMP_EEADDR+(set_num*2), set_temp[set_num]);
		eeprom_write(SINGLE_SETTEMP_EEADDR+(set_num*2)+1, set_temp[set_num]>>8);
	}
	
	if(switch_sensor_buff[set_num] != switch_sensor[set_num])
	{
		switch_sensor[set_num] = switch_sensor_buff[set_num];
		ctrl_command = SWITCH_SENSOR;
		global = SINGLE;
//		eeprom_write(SINGLE_SWSENSOR_EEADDR+set_num, switch_sensor[set_num]);
	}
	
	if(sensor_type_buff[set_num] != sensor_type[set_num])
	{
		sensor_type[set_num] = sensor_type_buff[set_num];
		ctrl_command = SENSOR_TYPE;
		eeprom_write(SINGLE_SENSORTYPE_EEADDR+set_num, sensor_type[set_num]);
	}
	
	if(set_name_buff != set_name[set_num])
	{
		set_name[set_num] = set_name_buff;
		
		eeprom_write(SET_NAME_EEADDR+(set_num*4), set_name[set_num]);
		eeprom_write(SET_NAME_EEADDR+(set_num*4)+1, set_name[set_num]>>8);
		eeprom_write(SET_NAME_EEADDR+(set_num*4)+2, set_name[set_num]>>16);
		eeprom_write(SET_NAME_EEADDR+(set_num*4)+3, set_name[set_num]>>24);
	}
	
	in_main_page = 1;
	update_run_temp_flag = 0;
}

void all_set_ok(void)
{
	uint8_t i = 0;
	static uint8_t all_set_num = 0;
	
	if(all_set_num <= 2)
	{
		all_temp = all_temp_buff;
		all_set(TEMP, all_temp);
		
		eeprom_write(ALL_SETTEMP_EEADDR, all_temp);
		eeprom_write(ALL_SETTEMP_EEADDR+1, all_temp>>8);
		
		for(i=0; i<12; i++)
		{
			set_temp[i] = all_temp;
			set_temp_buff[i] = all_temp;
			eeprom_write(SINGLE_SETTEMP_EEADDR+(i*2), set_temp[i]);
			eeprom_write(SINGLE_SETTEMP_EEADDR+(i*2)+1, set_temp[i]>>8);
		}
		
		if(temp_unit_buff != temp_unit)
		{
			temp_unit = temp_unit_buff;
			send_variables(TEMP_UINT_ADDR, (CELSIUS+temp_unit*FAHRENHEIT));
			eeprom_write(TEMP_UNIT_EEADDR, temp_unit);
		}
		
		in_main_page = 1;
	}
	
	if(all_set_num <= 5)
	{
		if(preheat_time_buff != preheat_time)
		{
			preheat_time = preheat_time_buff;
			
			all_set(PREHEAT_TIME, preheat_time);
			
			eeprom_write(PREHEAT_TIME_EEADDR, preheat_time);
		}
	}
	
	if(all_set_num <= 8)
	{
		if(all_sensor_type_buff != all_sensor_type)
		{
			all_sensor_type = all_sensor_type_buff;
			
			all_set(SENSOR_TYPE, all_sensor_type);
			eeprom_write(ALL_SENSORTYPE_EEADDR, all_sensor_type);		
				
			for(i=0; i<12; i++)
			{
				sensor_type[i] = all_sensor_type;
				sensor_type_buff[i] = all_sensor_type;
				eeprom_write(SINGLE_SENSORTYPE_EEADDR+i, sensor_type[i]);
			}
			send_variables(ALL_SENSOR_TYPE_ADDR, TYPE_J+(all_sensor_type*TYPE_K));
		}
	}
	
	all_set_num++;
	if(all_set_num >= 9)
	{
		all_set_num = 0;
		ctrl_command = READ_DATA_ALL;
	}
	
}

void pid_set_ok(void)
{
	p_value[set_pid_channel-1] = p_value_buff;
	i_value[set_pid_channel-1] = i_value_buff;
	d_value[set_pid_channel-1] = d_value_buff;
	
	ctrl_command = PID;
	
	eeprom_write(PID_P_EEADDR+((set_pid_channel-1)*2), p_value[set_pid_channel-1]);
	eeprom_write(PID_P_EEADDR+((set_pid_channel-1)*2)+1, p_value[set_pid_channel-1]>>8);
	
	eeprom_write(PID_I_EEADDR+((set_pid_channel-1)*2), i_value[set_pid_channel-1]);
	eeprom_write(PID_I_EEADDR+((set_pid_channel-1)*2)+1, i_value[set_pid_channel-1]>>8);

	eeprom_write(PID_D_EEADDR+((set_pid_channel-1)*2), d_value[set_pid_channel-1]);
	eeprom_write(PID_D_EEADDR+((set_pid_channel-1)*2)+1, d_value[set_pid_channel-1]>>8);
}

/*
**设置PID参数，通过串口1发送到控制办卡
*/
void set_pid(void)
{
	uint16_t crc = 0;
	uint8_t addr = 0;
	
	/*计算地址，前四位代表示板卡号，后四位表示*/
	addr = (set_pid_channel-1)/4 + 1;
	addr = (addr<<4) + ((set_pid_channel-1)%4) + 1;
	
	usart1_tx_buff[0] = 0xA5;
	usart1_tx_buff[1] = 0x5A;
	usart1_tx_buff[2] = 0x08;
	usart1_tx_buff[3] = PID;
	usart1_tx_buff[4] = addr;
	
	usart1_tx_buff[5] =(uint8_t)(p_value_buff>>8);
	usart1_tx_buff[6] =(uint8_t)(p_value_buff);
	
	usart1_tx_buff[7] =(uint8_t)(i_value_buff>>8);
	usart1_tx_buff[8] =(uint8_t)(i_value_buff);
	
	usart1_tx_buff[9] =(uint8_t)(d_value_buff>>8);
	usart1_tx_buff[10] =(uint8_t)(d_value_buff);
	
	crc = crc_check(usart1_tx_buff, 13);

	usart1_tx_buff[11] = crc & 0x00FF;
	usart1_tx_buff[12] = crc >> 8;

	usart1_send_str(usart1_tx_buff, 13);
	
	ctrl_command = READ_DATA_ALL;
}


void single_set_back(void)
{
	if(set_temp_buff[set_num] != set_temp[set_num])
	{
		set_temp_buff[set_num] = set_temp[set_num];
	}
	
	if(switch_sensor_buff[set_num] != switch_sensor[set_num])
	{
		switch_sensor_buff[set_num] = switch_sensor[set_num];
	}
	
	if(sensor_type_buff[set_num] != sensor_type[set_num])
	{
		sensor_type_buff[set_num] = sensor_type[set_num];
	}
	
	in_main_page = 1;
	update_run_temp_flag = 0;
}

void all_set_back(void)
{
	if(all_temp_buff != all_temp)
	{
		all_temp_buff = all_temp;
		send_variables(ALL_SET_TEMP, all_temp);
	}
	
	if(preheat_time_buff != preheat_time)
	{
		preheat_time_buff = preheat_time;
		send_variables(SET_PREHEAT_TIME, preheat_time);
	}
	
	if(all_sensor_type_buff != all_sensor_type)
	{
		all_sensor_type_buff = all_sensor_type;
		send_variables(ALL_SENSOR_TYPE_ADDR, TYPE_J+(all_sensor_type*TYPE_K));
	}
	
	if(temp_unit_buff != temp_unit)
	{
		temp_unit_buff = temp_unit;
		send_variables(TEMP_UINT_ADDR, (CELSIUS+temp_unit*FAHRENHEIT));
	}
	
	in_main_page = 1;
}

void switch_all_sensor(uint16_t sta)
{
	uint8_t i = 0;
	uint16_t crc = 0;
	static uint8_t global_set_num = 1;
	
	usart1_tx_buff[4] = global_set_num << 4;
	
	if(global_set_num == 1)
	{
		usart1_tx_buff[0] = 0xA5;
		usart1_tx_buff[1] = 0x5A;
		usart1_tx_buff[2] = 0x04;
		usart1_tx_buff[3] = 0x06;
		usart1_tx_buff[5] = 0x00;
		usart1_tx_buff[6] = sta;
		
		pre_system_sta = sta;
		
		for(i=0; i<12; i++)
		{
			switch_sensor[i] = sta;
			switch_sensor_buff[i] = sta;
//			eeprom_write(SINGLE_SWSENSOR_EEADDR+i, switch_sensor[i]);
		}		
	}
	
	crc = crc_check(usart1_tx_buff, 9);
	
	usart1_tx_buff[7] = crc & 0x00FF;
	usart1_tx_buff[8] = crc >> 8;
	
	usart1_send_str(usart1_tx_buff, 9);
	
	global_set_num++;
	
	if(global_set_num >= 4)
	{
		global_set_num = 1;
		ctrl_command = READ_DATA_ALL;
	}
}

void get_set_name(void)
{
	set_name_buff = usart0_rx_buff[7];
	set_name_buff = (set_name_buff<<8) | usart0_rx_buff[8];
	set_name_buff = (set_name_buff<<8) | usart0_rx_buff[9];
	set_name_buff = (set_name_buff<<8) | usart0_rx_buff[10];
}

void single_set(uint8_t command, uint16_t value)
{
	uint16_t crc = 0;
	uint8_t addr = 0;
	
	if(temp_unit_buff  ==  1)
	{
		value = ((value - 32)*10/18);
	}
	
	addr = (set_num/4) + 1;
	addr = (addr<<4) | ((set_num%4)+1);

	usart1_tx_buff[0] = 0xA5;
	usart1_tx_buff[1] = 0x5A;
	usart1_tx_buff[2] = 0x04;
	usart1_tx_buff[3] = command;
	usart1_tx_buff[4] = addr;
	usart1_tx_buff[5] = (value>>8);
	usart1_tx_buff[6] = value;

	crc = crc_check(usart1_tx_buff, 9);

	usart1_tx_buff[7] = crc & 0x00FF;
	usart1_tx_buff[8] = crc >> 8;

	usart1_send_str(usart1_tx_buff, 9);	
	
	ctrl_command = READ_DATA_ALL;
}

void all_set(uint8_t command, uint16_t value)
{
	uint16_t crc = 0;
	static uint8_t slave_num = 1;
	
	if(temp_unit_buff  ==  1)
	{
		value = ((value - 32)*10/18);
	}
		
	usart1_tx_buff[0] = 0xA5;
	usart1_tx_buff[1] = 0x5A;
	usart1_tx_buff[2] = 0x04;
	usart1_tx_buff[3] = command;
	usart1_tx_buff[5] = (value>>8);
	usart1_tx_buff[6] = value;
	
	usart1_tx_buff[4] = slave_num<<4;
	
	crc = crc_check(usart1_tx_buff, 9);
	
	usart1_tx_buff[7] = crc & 0x00FF;
	usart1_tx_buff[8] = crc >> 8;
	
	usart1_send_str(usart1_tx_buff, 9);
	
	slave_num++;
	if(slave_num >= 4)
	{
		slave_num = 1;
	}
}

void update_single_set_page(void)
{
	in_main_page = 0;
	
	update_run_temp_flag = 1;
	run_temp_page = 1;
	
	send_variables(SINGLE_NUM_ADDR, set_num+1);						//序号显示
	send_variables(SINGLE_RUNTEMP_ADDR, run_temp[set_num]+temp_unit*((run_temp[set_num]*8/10)+32));			//运行温度显示
	send_variables(SINGLE_SETTEMP_ADDR, set_temp[set_num]);			//设定温度显示
	send_variables(SINGLE_SET_TEMP, set_temp[set_num]);				//温度设定输入框显示
	send_variables(SINGLE_SET_SWITCH, switch_sensor[set_num]);		//传感器开启/关闭状态
	send_variables(SINGLE_SENSORTYPE_ADDR, TYPE_J + (sensor_type[set_num]*TYPE_K));	//DANDUPAGE传感器类型选择显示
	
	send_name(SINGLE_NAME_ADDR, set_name[set_num]);
	
	send_name(SINGLE_SET_NAME, set_name[set_num]);
	
	set_name_buff = set_name[set_num];
}

void update_pid_page(uint8_t channel)
{
	set_pid_channel = channel;
	
	if(channel == 1)
	{
		send_variables(PID_CHANNEL, 1);
	}
	
	send_variables(PID_P_ADDR, p_value[channel-1]);
	send_variables(PID_I_ADDR, i_value[channel-1]);
	send_variables(PID_D_ADDR, d_value[channel-1]);
	
	send_variables(PID_P, p_value[channel-1]);
	send_variables(PID_I, i_value[channel-1]);
	send_variables(PID_D, d_value[channel-1]);	
	
	p_value_buff = p_value[channel-1];
	i_value_buff = i_value[channel-1];
	d_value_buff = d_value[channel-1];
}

void update_curve_page(void)
{
	uint8_t i = 0;
	uint8_t count = 0;
	uint16_t time_line[10] = {2, 4, 6, 8, 10, 12, 14, 16, 18, 20};
	
	update_run_temp_flag = 1;
	run_temp_page = 0;
	
	if(curve_time_level >= 4)
	{
		curve_time_level = 3;
	}
	else if(curve_time_level < 0)
	{
		curve_time_level = 0;
	}
	
	if(curve_page_num >= 12)
	{
		curve_page_num = 0;
	}
	else if(curve_page_num < 0)
	{
		curve_page_num = 11;
	}
	
	count = 10*(curve_time_level+1);
		
	CLOSE_CURVE;	
	clear_curve_buff(CHANNEL0_BUFF);
	
	for(i=0; i<10; i++)
	{
		time_line[i] *=  (curve_time_level+1);
		send_variables(CURVE_TIMELINE_ADDR+(i*2), time_line[i]);
	}
	
	send_variables(CURVE_PAGE_NUM_ADDR, curve_page_num+1);
	send_variables(CURVE_PAGE_RUNTEMP_ADDR, run_temp[curve_page_num] +
											temp_unit*(run_temp[curve_page_num]*8/10 + 32));	//运行温度
	send_variables(CURVE_PAGE_SETTEMP_ADDR, set_temp[curve_page_num]);							//设定温度
	send_variables(CURVE_PAGE_OUTRATE_ADDR, output_rate[curve_page_num]);
	send_variables(CURVE_PAGE_SENSORTYPE_ADDR, TYPE_J+(sensor_type[curve_page_num]*TYPE_K));
	send_name(CURVE_PAGE_NAME_ADDR, set_name[curve_page_num]);
	
	OPEN_CURVE(count);
}

void get_time_ctrl_value(void)
{
	t1[pre_iqr] = t1_buff;
	t2[pre_iqr] = t2_buff;
	t3[pre_iqr] = t3_buff;
	t4[pre_iqr] = t4_buff;
	
	eeprom_write(TIME_CTRL_T1_EEADDR+(pre_iqr*2), t1[pre_iqr]);
	eeprom_write(TIME_CTRL_T1_EEADDR+(pre_iqr*2)+1, t1[pre_iqr]>>8);
		
	eeprom_write(TIME_CTRL_T2_EEADDR+(pre_iqr*2), t2[pre_iqr]);
	eeprom_write(TIME_CTRL_T2_EEADDR+(pre_iqr*2)+1, t2[pre_iqr]>>8);
			
	eeprom_write(TIME_CTRL_T3_EEADDR+(pre_iqr*2), t3[pre_iqr]);
	eeprom_write(TIME_CTRL_T3_EEADDR+(pre_iqr*2)+1, t3[pre_iqr]>>8);
			
	eeprom_write(TIME_CTRL_T4_EEADDR+(pre_iqr*2), t4[pre_iqr]);
	eeprom_write(TIME_CTRL_T4_EEADDR+(pre_iqr*2)+1, t4[pre_iqr]>>8);
	
	send_variables(TIME_CTRL_T1_ADDR, t1[pre_iqr]);
	send_variables(TIME_CTRL_T2_ADDR, t2[pre_iqr]);
	send_variables(TIME_CTRL_T3_ADDR, t3[pre_iqr]);
	send_variables(TIME_CTRL_T4_ADDR, t4[pre_iqr]);
}

void update_time_ctrl_page(void)
{
	
	clear_curve_buff(CHANNEL0_BUFF);
	
	t1_buff = t1[pre_iqr];
	t2_buff = t2[pre_iqr];
	t3_buff = t3[pre_iqr];
	t4_buff = t4[pre_iqr];
	
	send_variables(TIME_CTRL_T1, t1[pre_iqr]);
	send_variables(TIME_CTRL_T2, t2[pre_iqr]);
	send_variables(TIME_CTRL_T3, t3[pre_iqr]);
	send_variables(TIME_CTRL_T4, t4[pre_iqr]);
	send_variables(TIME_CTRL_T1_ADDR, t1[pre_iqr]);
	send_variables(TIME_CTRL_T2_ADDR, t2[pre_iqr]);
	send_variables(TIME_CTRL_T3_ADDR, t3[pre_iqr]);
	send_variables(TIME_CTRL_T4_ADDR, t4[pre_iqr]);
}

void update_alarm_page(uint8_t page_num)
{
	uint8_t i = 0;
	uint8_t alarm_msg_num = 0;
	uint16_t sensor_num = 0;
	
	for(i=0; i<7; i++)
	{
		send_variables(ALARM_PAGE_NUM1_ADDR+(i*2), i+((page_num-1)*7)+1);
	}
	
	for(i=0; i<12; i++)
	{
		if(alarm_type[i] != 0)
		{
			sensor_num = i;
			send_alarm_msg(alarm_type[i], alarm_msg_num, sensor_num);
			alarm_msg_num++;
		}
	}
	
	for(; alarm_msg_num<7; alarm_msg_num++)
	{
		clear_alarm_msg(alarm_msg_num);	
	}
}

void switch_language(void)
{
	if(pre_language == 1)
	{
		change_page(3);		//暂定英文页面为13
	}
	else
	{
		change_page(1);
	}
	
	eeprom_write(PRE_LANGUAGE_EEADDR, pre_language);
}

void change_page(uint8_t page_num)
{
	uint16_t crc = 0;
	
	usart0_tx_buff[0] = 0xA5;
	usart0_tx_buff[1] = 0x5A;
	usart0_tx_buff[2] = 0x04;			//4.3寸屏为4  5寸屏为6
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
	usart0_tx_buff[8] = name >>8;
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
	usart0_tx_buff[5] = value>>8;
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
	usart0_tx_buff[4] = (ALAEM_MSG1_ADDR+(msg_num*16))>>8;
	usart0_tx_buff[5] = (ALAEM_MSG1_ADDR+(msg_num*16)) & 0x00FF;
	
	for(i=0; i<16; i++)
	{
		usart0_tx_buff[i+6] = alarm_msg[type-1][i];
	}
	
	crc = crc_check(usart0_tx_buff, 24);
	
	usart0_tx_buff[22] = crc & 0x00FF;
	usart0_tx_buff[23] = crc >> 8;
	
	usart0_send_str(usart0_tx_buff, 24);
	
	if(sen_num <= 8)
	{
		sensor_num = 0x2300 | (sen_num+49);
	}
	else
	{
		sensor_num = 0x2300 | (sen_num+56);	
	}
	
	send_variables(ALARM_DEVICE_NUM1_ADDR+(msg_num*2), sensor_num);
}

void send_request_all(uint8_t addr)
{
	uint16_t crc = 0;
	
	usart1_tx_buff[0] = 0xA5;
	usart1_tx_buff[1] = 0x5A;
	usart1_tx_buff[2] = 0x02;
	usart1_tx_buff[3] = 0x01;
	usart1_tx_buff[4] = (addr<<4);
	
	crc = crc_check(usart1_tx_buff, 7);
	
	usart1_tx_buff[5] = crc & 0x00FF;
	usart1_tx_buff[6] = crc >> 8;
	 
	usart1_send_str(usart1_tx_buff, 7);
}

void get_data_all()
{
	uint8_t slave_num = 0;
	
	slave_num = (usart1_rx_buff[4]>>4);
	
	output_rate[0+((slave_num-1)*4)] = usart1_rx_buff[5];
	output_rate[1+((slave_num-1)*4)] = usart1_rx_buff[6];
	output_rate[2+((slave_num-1)*4)] = usart1_rx_buff[7];
	output_rate[3+((slave_num-1)*4)] = usart1_rx_buff[8];
	
	sensor_sta[0+((slave_num-1)*4)] = usart1_rx_buff[9];
	sensor_sta[1+((slave_num-1)*4)] = usart1_rx_buff[10];
	sensor_sta[2+((slave_num-1)*4)] = usart1_rx_buff[11];
	sensor_sta[3+((slave_num-1)*4)] = usart1_rx_buff[12];
	
	run_temp[0+((slave_num-1)*4)] = usart1_rx_buff[13];
	run_temp[0+((slave_num-1)*4)] = (run_temp[0+((slave_num-1)*4)]<<8) | usart1_rx_buff[14];
	run_temp[1+((slave_num-1)*4)] = usart1_rx_buff[15];
	run_temp[1+((slave_num-1)*4)] = (run_temp[1+((slave_num-1)*4)]<<8) | usart1_rx_buff[16];
	run_temp[2+((slave_num-1)*4)] = usart1_rx_buff[17];
	run_temp[2+((slave_num-1)*4)] = (run_temp[2+((slave_num-1)*4)]<<8) | usart1_rx_buff[18];
	run_temp[3+((slave_num-1)*4)] = usart1_rx_buff[19];
	run_temp[3+((slave_num-1)*4)] = (run_temp[3+((slave_num-1)*4)]<<8) | usart1_rx_buff[20];
	
	alarm_type[0+((slave_num-1)*4)] = usart1_rx_buff[21];
	alarm_type[1+((slave_num-1)*4)] = usart1_rx_buff[22];
	alarm_type[2+((slave_num-1)*4)] = usart1_rx_buff[23];
	alarm_type[3+((slave_num-1)*4)] = usart1_rx_buff[24];
}


void clear_alarm_msg(uint8_t msg_num)
{
	uint8_t i = 0;
	uint16_t crc = 0;
	
	usart0_tx_buff[0] = 0xA5;
	usart0_tx_buff[1] = 0x5A;
	usart0_tx_buff[2] = 0x13;
	usart0_tx_buff[3] = 0x82;
	usart0_tx_buff[4] = (ALAEM_MSG1_ADDR+(msg_num*16)) >> 8;
	usart0_tx_buff[5] = (ALAEM_MSG1_ADDR+(msg_num*16)) & 0x00FF;
	
	for(i=0; i<16; i++)
	{
		usart0_tx_buff[i+6] = 0x00;
	}
	
	crc = crc_check(usart0_tx_buff, 24);
	
	usart0_tx_buff[22] = crc & 0x00FF;
	usart0_tx_buff[23] = crc >> 8;
	
	usart0_send_str(usart0_tx_buff, 24);
	
	send_variables(ALARM_DEVICE_NUM1_ADDR+(msg_num*2), 0x0000);
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
