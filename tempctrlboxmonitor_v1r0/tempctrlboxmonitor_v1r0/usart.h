/*
 * uart.h
 *
 * Created: 2018-04-03 09:32:17
 *  Author: chenlong
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "dgus.h"
#include "timer.h"
#include "gpio.h"

#define MYUBRR(baud)	 ((F_CPU)/(baud*16UL)-1)

//定义USART端口方向	
#define	USART1_TX_PIN_OUTPUT	DDRD |= (1<<3)
#define USART1_RX_PIN_INPUT		DDRD &= ~(1<<2)
#define USART0_TX_PIN_OUTPUT	DDRE |= (1<<1)
#define USART0_RX_PIN_INPUT		DDRE &= ~(1<<0)

#define RX1_COMMAND				usart1_rx_buff[3]			//usart1接收数据包中的命令
#define RX0_COMMAND				usart0_rx_buff[3]			//usart0接收数据包中的命令

#define ACCESS_VARIABLE			0x83						//USART0访问变量命令

#define READ_DATA_ALL			0x01						//USART1读取全部数据命令

//宏定义usart1接收变量地址
#define MASTER_SWITCH			0x0140
#define SINGLE_SET_SENSOR_TYPE	0x0142
#define SINGLE_SET_SWITCH		0x0144
#define SINGLE_SET_TEMP			0x0146
#define SINGLE_SET_NAME			0x0158   				//~~!!!!!!!!!!!!!!z  0x0158 - 0x015F
#define ALL_SET_TEMP			0x0148
#define SET_PREHEAT_TIME		0x014A
#define ALL_SET_SENSOR_TYPE		0x014C
#define SET_TEMP_UNIT			0x014E
#define PID_CHANNEL				0x0150
#define PID_P					0x0152
#define PID_I					0x0154
#define PID_D					0x0156
#define TIME_CTRL_T1			0x0160
#define TIME_CTRL_T2			0x0162
#define TIME_CTRL_T3			0x0164
#define TIME_CTRL_T4			0x0166
#define TIME_CTRL_IQR			0x0168
#define KEY_ADDR				0x01FF

extern uint8_t usart1_rx_buff[200];
extern uint8_t usart1_rx_end;
extern uint8_t usart1_rx_lenth;
extern uint8_t usart1_rx_cnt;
extern uint8_t usart1_tx_buff[200];

extern uint8_t usart0_rx_buff[200];
extern uint8_t usart0_tx_buff[200];
extern uint8_t usart0_rx_end;
extern uint8_t usart0_rx_lenth;
extern uint8_t usart0_rx_cnt;

extern uint16_t all_temp_buff;
extern uint16_t preheat_time_buff;
extern uint16_t all_sensor_type_buff;
extern uint16_t temp_unit_buff;

extern uint16_t set_temp_buff[12];
extern uint16_t switch_sensor_buff[12];
extern uint16_t sensor_type_buff[12];

extern uint16_t p_value_buff;
extern uint16_t i_value_buff;
extern uint16_t d_value_buff;

extern uint16_t t1_buff;
extern uint16_t t2_buff;
extern uint16_t t3_buff;
extern uint16_t t4_buff;


int usart0_deal(void);
int usart1_deal(void);
void usart0_init(uint16_t ubbr);
void usart1_init(uint16_t ubrr1);
void usart2_init(uint16_t baud);

void usart0_send_char(uint8_t data);
void usart0_send_str(uint8_t *p, uint8_t data_size);
void usart1_send_char(uint8_t data);
void usart1_send_str(uint8_t *p, uint8_t data_size);

uint16_t crc_check(uint8_t *p, uint8_t dataSize);

#endif /* UART_H_ */