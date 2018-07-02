/*
* uart.c
*
* Created: 2018-04-03 09:31:55
*  Author: chenlong
*/
#include "usart.h"

uint8_t usart0_rx_buff[200] = {0};
uint8_t usart0_tx_buff[200] = {0};

uint8_t usart0_rx_end = 0;
uint8_t usart0_rx_lenth = 0;
uint8_t usart0_rx_cnt = 0;

uint8_t usart1_rx_buff[200] = {0};
uint8_t usart1_tx_buff[200] = {0};

uint8_t usart1_rx_end = 0;
uint8_t usart1_rx_lenth = 0;
uint8_t usart1_rx_cnt = 0;

uint8_t usart2_tx_buff[200] = {0};

uint8_t usart2_buff = 0x00;
uint8_t usart2_sta;

uint16_t all_temp_buff = 100;
uint16_t preheat_time_buff = 3;
uint16_t all_sensor_type_buff = 0;
uint16_t temp_unit_buff = 0;

uint16_t set_temp_buff[MAX_IQR_QUANTITY];
uint16_t switch_sensor_buff[MAX_IQR_QUANTITY] = {0}; //默认状态为关闭
uint16_t sensor_type_buff[MAX_IQR_QUANTITY] = {0};

uint16_t p_value_buff = 100;
uint16_t i_value_buff = 100;
uint16_t d_value_buff = 100;

uint16_t time_ctrl_value_buff[4][8][4];

ISR(USART0_RX_vect)
{
	usart0_rx_buff[usart0_rx_cnt++] = UDR0;

	if (usart0_rx_cnt >= 200)
	{
		usart0_rx_lenth = usart0_rx_cnt;
		usart0_rx_cnt = 0;
	}
}

ISR(USART1_RX_vect)
{
	usart1_rx_buff[usart1_rx_cnt++] = UDR1;

	if (usart1_rx_cnt >= 200)
	{
		usart1_rx_lenth = usart1_rx_cnt;
		usart1_rx_cnt = 0;
	}
}

int usart0_deal(void)
{
	uint16_t crc_data = 0;

	uint16_t variable_addr = 0;
	uint16_t variable = 0;

	crc_data = usart0_rx_buff[usart0_rx_lenth - 1];
	crc_data = (crc_data << 8) + usart0_rx_buff[usart0_rx_lenth - 2]; //读取接收数据后两个字节的校验值

	variable_addr = usart0_rx_buff[4];
	variable_addr = (variable_addr << 8) | usart0_rx_buff[5]; //读取变量地址

	variable = usart0_rx_buff[usart0_rx_lenth - 4];
	variable = (variable << 8) | usart0_rx_buff[usart0_rx_lenth - 3]; //读取变量值

	if ((usart0_rx_buff[0] != 0xA5) || (usart0_rx_buff[1] != 0x5A)) //判断数据头是否正确
	{
		usart0_rx_end = 0;
		return 0;
	}

	if ((usart0_rx_buff[2] + 5) != usart0_rx_lenth) //判断数据长度是否正确
	{
		usart0_rx_end = 0;
		return 0;
	}

	if (crc_data != crc_check(usart0_rx_buff, usart0_rx_lenth)) //判断crc校验是否正确
	{
		usart0_rx_end = 0;
		return 0;
	}

	if (RX0_COMMAND == ACCESS_VARIABLE)
	{
		switch (variable_addr)
		{
		case KEY_ADDR:
			key_action(variable);
			break; //如果变量地址为按键变量地址  执行按键动作
		case MASTER_SWITCH:
			pre_system_sta = variable;
			all_set(SWITCH_SENSOR, variable);
/*			all_set_flag = 1;*/
/*			ctrl_command[ctrl_index++] = SWITCH_SENSOR;*/
			// switch_all_sensor(variable);
			break;
		case SINGLE_SET_FOLLOW:
			follow_sta_buff[set_num] = variable;
			break;
		case SINGLE_SET_SENSOR_TYPE:
			sensor_type_buff[set_num] = variable;
			send_variables(SINGLE_SENSORTYPE_ADDR,
						   TYPE_J + (sensor_type_buff[set_num] * TYPE_K));
			break;
		case SINGLE_SET_SWITCH:
			switch_sensor_buff[set_num] = variable;
			break;
		case SINGLE_SET_TEMP:
			if(temp_unit_buff)
			{
				variable = ((variable - 32) * 10 / 18);
			}
			set_temp_buff[set_num] = variable;
			break;
		case SINGLE_SET_NAME:
			set_name_buff = get_name();
			break;
		case ALL_SET_TEMP:
			if (temp_unit_buff)
			{
				variable = ((variable - 32) * 10 / 18);
			}
			all_temp_buff = variable;
			break;
		case SET_PREHEAT_TIME:
			preheat_time_buff = variable;
			break;
		case ALL_SET_SENSOR_TYPE:
			all_sensor_type_buff = variable;
			send_variables(ALL_SENSOR_TYPE_ADDR,
						   TYPE_J + (all_sensor_type_buff * TYPE_K));
			break;
		case SET_TEMP_UNIT:
			temp_unit_buff = variable;
			send_variables(TEMP_UINT_ADDR,
						   (CELSIUS + temp_unit_buff * FAHRENHEIT));
			if(temp_unit_buff == 1)
			{
				send_variables(ALL_SET_TEMP, all_temp + temp_unit_buff * (all_temp * 8 / 10 + 32));
			}
			else
			{
				send_variables(ALL_SET_TEMP, all_temp);
			}
			break;
			//		case PID_CHANNEL:
			//			update_pid_page(variable);	//更改屏幕最大为24来改变最大通道数（最好改为按键返回）
			//			break;
		case PID_P:
			p_value_buff = variable;
			break;
		case PID_I:
			i_value_buff = variable;
			break;
		case PID_D:
			d_value_buff = variable;
			break;

		case IQR1_T1:
			time_ctrl_value_buff[module_num - 1][0][0] = variable;
			break;
		case IQR1_T2:
			time_ctrl_value_buff[module_num - 1][0][1] = variable;
			break;
		case IQR1_T3:
			time_ctrl_value_buff[module_num - 1][0][2] = variable;
			break;
		case IQR1_T4:
			time_ctrl_value_buff[module_num - 1][0][3] = variable;
			break;

		case IQR2_T1:
			time_ctrl_value_buff[module_num - 1][1][0] = variable;
			break;
		case IQR2_T2:
			time_ctrl_value_buff[module_num - 1][1][1] = variable;
			break;
		case IQR2_T3:
			time_ctrl_value_buff[module_num - 1][1][2] = variable;
			break;
		case IQR2_T4:
			time_ctrl_value_buff[module_num - 1][1][3] = variable;
			break;

		case IQR3_T1:
			time_ctrl_value_buff[module_num - 1][2][0] = variable;
			break;
		case IQR3_T2:
			time_ctrl_value_buff[module_num - 1][2][1] = variable;
			break;
		case IQR3_T3:
			time_ctrl_value_buff[module_num - 1][2][2] = variable;
			break;
		case IQR3_T4:
			time_ctrl_value_buff[module_num - 1][2][3] = variable;
			break;

		case IQR4_T1:
			time_ctrl_value_buff[module_num - 1][3][0] = variable;
			break;
		case IQR4_T2:
			time_ctrl_value_buff[module_num - 1][3][1] = variable;
			break;
		case IQR4_T3:
			time_ctrl_value_buff[module_num - 1][3][2] = variable;
			break;
		case IQR4_T4:
			time_ctrl_value_buff[module_num - 1][3][3] = variable;
			break;

		case IQR5_T1:
			time_ctrl_value_buff[module_num - 1][4][0] = variable;
			break;
		case IQR5_T2:
			time_ctrl_value_buff[module_num - 1][4][1] = variable;
			break;
		case IQR5_T3:
			time_ctrl_value_buff[module_num - 1][4][2] = variable;
			break;
		case IQR5_T4:
			time_ctrl_value_buff[module_num - 1][4][3] = variable;
			break;

		case IQR6_T1:
			time_ctrl_value_buff[module_num - 1][5][0] = variable;
			break;
		case IQR6_T2:
			time_ctrl_value_buff[module_num - 1][5][1] = variable;
			break;
		case IQR6_T3:
			time_ctrl_value_buff[module_num - 1][5][2] = variable;
			break;
		case IQR6_T4:
			time_ctrl_value_buff[module_num - 1][5][3] = variable;
			break;

		case IQR7_T1:
			time_ctrl_value_buff[module_num - 1][6][0] = variable;
			break;
		case IQR7_T2:
			time_ctrl_value_buff[module_num - 1][6][1] = variable;
			break;
		case IQR7_T3:
			time_ctrl_value_buff[module_num - 1][6][2] = variable;
			break;
		case IQR7_T4:
			time_ctrl_value_buff[module_num - 1][6][3] = variable;
			break;

		case IQR8_T1:
			time_ctrl_value_buff[module_num - 1][7][0] = variable;
			break;
		case IQR8_T2:
			time_ctrl_value_buff[module_num - 1][7][1] = variable;
			break;
		case IQR8_T3:
			time_ctrl_value_buff[module_num - 1][7][2] = variable;
			break;
		case IQR8_T4:
			time_ctrl_value_buff[module_num - 1][7][3] = variable;
			break;
		case TEMPLATE_FIND_NAME:
			tp_find_name = get_name();	
			break;
		case TEMPLATE_SAVE_NAME:
			tp_save_name = get_name();
			break;
		case TEMPLATE_NAME1:
			set_template_name(pre_first_tpnum, get_name());
			break;
		case TEMPLATE_NAME2:
			set_template_name(pre_first_tpnum+1, get_name());
			break;
		case TEMPLATE_NAME3:
			set_template_name(pre_first_tpnum+2, get_name());
			break;
		case TEMPLATE_NAME4:
			set_template_name(pre_first_tpnum+3, get_name());
			break;
		case TEMPLATE_NAME5:
			set_template_name(pre_first_tpnum+4, get_name());
			break;
		default:
			break;
		}
	}

	return 0;
}

int usart1_deal(void)
{
	uint16_t crc_data = 0;

	crc_data = usart1_rx_buff[usart1_rx_lenth - 1];
	crc_data = (crc_data << 8) + usart1_rx_buff[usart1_rx_lenth - 2];

	if ((usart1_rx_buff[0] != 0xA5) || (usart1_rx_buff[1] != 0x5A))
	{
		usart1_rx_end = 0;
		return 0;
	}

	if ((usart1_rx_buff[2] + 5) != usart1_rx_lenth)
	{
		usart1_rx_end = 0;
		return 0;
	}

	// 	if (crc_data != crc_check(usart1_rx_buff, usart1_rx_lenth))
	// 	{
	// 		usart1_rx_end = 0;
	// 		return 0;
	// 	}

	if (RX1_COMMAND == READ_DATA_ALL)
	{
		get_data_all();
	}
	else if (RX1_COMMAND == READ_SETTING_DATA)
	{
		get_setting_data(usart1_rx_buff[4]);
	}

	return 0;
}

void usart0_send_char(uint8_t data)
{
	while (!(UCSR0A & (1 << UDRE0)))
		;
	UDR0 = data;
}

void usart0_send_str(uint8_t *str, uint8_t data_size)
{
	uint8_t i = 0;

	for (i = 0; i < data_size; i++)
	{
		usart0_send_char(*str++);
	}
}

void usart1_send_char(uint8_t data)
{
	while (!(UCSR1A & (1 << UDRE1)))
		; //等待发送寄存器为空
	UDR1 = data;
}

void usart1_send_str(uint8_t *str, uint8_t data_size)
{
	uint8_t i = 0;

	for (i = 0; i < data_size; i++)
	{
		usart1_send_char(*str++);
	}
}

void usart2_send_char(uint8_t data)
{
	usart2_buff = data; //把数据保存到发送数据缓冲区

	usart2_sta = USART2_IN_TX; //状态更改为发送状态

	USART2_TX_PIN_RESET; //给起始位

	TCNT3 = 0x0000; //定时器3计数器清零
	TCCR3B |= 0x01; //启动定时器
}

void usart2_send_str(uint8_t *str, uint8_t data_size)
{
	uint8_t i = 0;

	for (i = 0; i < data_size; i++)
	{
		usart2_send_char(*str++);
		//		_delay_us(1200);S
		while (usart2_sta != USART2_TX_END)
			;
	}
}

uint16_t crc_check(uint8_t *p, uint8_t data_size)
{
	uint16_t crc = 0xFFFF;
	uint8_t i, j;

	for (j = 0; j < (data_size - 5); j++)
	{
		crc = crc ^ *(p + j + 3);
		for (i = 0; i < 8; i++)
		{
			if ((crc & 0x0001) > 0)
			{
				crc >>= 1;
				crc ^= 0xA001;
			}
			else
				crc >>= 1;
		}
	}

	return crc;
}

void usart0_init(uint16_t ubrr0) //屏幕通讯使用
{
	USART0_TX_PIN_OUTPUT;
	USART0_RX_PIN_INPUT; //配置引脚方向

	UBRR0H = (uint8_t)(ubrr0 >> 8);
	UBRR0L = (uint8_t)ubrr0;

	//	UBRR0H = 0;
	//	UBRR0L = 103;									//波特率9600，晶振16M
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);			   // Set frame format: 8data, 1stop bit 无校验,异步正常模式
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); //使能发送、接收；使能接收中断
}

void usart1_init(uint16_t ubrr1) //设备通讯使用
{
	USART1_TX_PIN_OUTPUT;
	USART1_RX_PIN_INPUT; //配置引脚方向

	UBRR1H = (uint8_t)(ubrr1 >> 8);
	UBRR1L = (uint8_t)ubrr1;

	//	UBRR1H = 0;
	//	UBRR1L = 103;						//波特率9600，晶振16M
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);				  // Set frame format: 8data, 1stop bit 无校验
	UCSR1B = (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1); //使能接收,发送
}

/*使用IO口+TIMER3模拟串口*/
void usart2_init(uint16_t baud)
{
	/*io口模拟uart相关IO初始化*/
	USART2_TX_PIN_OUTPUT; //TX脚配置为输出
	USART2_RX_PIN_INPUT;
	USART2_RX_PIN_PULLUP; //RX配置为输入上拉

	USART2_TX_PIN_SET; //TX脚输出高

	TCCR3B |= 0x08; //WGM32位置1
	TCCR3B &= 0xF8; //关闭定时器（清除时钟选择位 CS30 CS31 CS32）

	OCR3A = 16000000UL / baud; //定时时间为波特率的1/2（这里不启动定时器，启动时需要定时器时钟不分频 TCCR3B |= 0x01）

	ETIMSK |= 0x10; //定时器3 A输出比较中断使能
}
