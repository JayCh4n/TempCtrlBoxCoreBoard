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
#include <util/delay.h>

#define MYUBRR(baud) ((F_CPU) / (baud * 16UL) - 1)

//����USART�˿ڷ���
#define USART1_TX_PIN_OUTPUT DDRD |= (1 << 3)
#define USART1_RX_PIN_INPUT DDRD &= ~(1 << 2)
#define USART0_TX_PIN_OUTPUT DDRE |= (1 << 1)
#define USART0_RX_PIN_INPUT DDRE &= ~(1 << 0)

#define RX1_COMMAND usart1_rx_buff[3] //usart1�������ݰ��е�����
#define RX0_COMMAND usart0_rx_buff[3] //usart0�������ݰ��е�����

/* usart2��ǰ״̬���� */
#define USART2_IN_TX 0
#define USART2_IN_RX 1
#define USART2_TX_END 3
#define USART2_RX_END 4

#define ACCESS_VARIABLE 0x83 //USART0���ʱ�������

#define READ_DATA_ALL 0x01 //USART1��ȡȫ����������

//�궨��usart1���ձ�����ַ
#define MASTER_SWITCH 0x0140
#define SINGLE_SET_SENSOR_TYPE 0x0142
#define SINGLE_SET_SWITCH 0x0144
#define SINGLE_SET_TEMP 0x0146
#define SINGLE_SET_NAME 0x0158 //~~!!!!!!!!!!!!!!z  0x0158 - 0x015F
#define ALL_SET_TEMP 0x0148
#define SET_PREHEAT_TIME 0x014A
#define ALL_SET_SENSOR_TYPE 0x014C
#define SET_TEMP_UNIT 0x014E

#define PID_CHANNEL 0x0150
#define PID_P 0x0152
#define PID_I 0x0154
#define PID_D 0x0156

/*�佺ʱ����Ʋ���  ����+��ʾ��ַ*/
#define IQR1_T1 0x0162
#define IQR1_T2 0x0164
#define IQR1_T3 0x0166
#define IQR1_T4 0x0168

#define IQR2_T1 0x016A
#define IQR2_T2 0x016C
#define IQR2_T3 0x016E
#define IQR2_T4 0x0170

#define IQR3_T1 0x0172
#define IQR3_T2 0x0174
#define IQR3_T3 0x0176
#define IQR3_T4 0x0178

#define IQR4_T1 0x017A
#define IQR4_T2 0x017C
#define IQR4_T3 0x017E
#define IQR4_T4 0x0180

#define IQR5_T1 0x0182
#define IQR5_T2 0x0184
#define IQR5_T3 0x0186
#define IQR5_T4 0x0188

#define IQR6_T1 0x018A
#define IQR6_T2 0x018C
#define IQR6_T3 0x018E
#define IQR6_T4 0x0190

#define IQR7_T1 0x0192
#define IQR7_T2 0x0194
#define IQR7_T3 0x0196
#define IQR7_T4 0x0198

#define IQR8_T1 0x019A
#define IQR8_T2 0x019C
#define IQR8_T3 0x019E
#define IQR8_T4 0x01A0

#define KEY_ADDR 0x01FF

extern uint8_t usart0_rx_buff[200];
extern uint8_t usart0_tx_buff[200];
extern uint8_t usart0_rx_end;
extern uint8_t usart0_rx_lenth;
extern uint8_t usart0_rx_cnt;

extern uint8_t usart1_rx_buff[200];
extern uint8_t usart1_rx_end;
extern uint8_t usart1_rx_lenth;
extern uint8_t usart1_rx_cnt;
extern uint8_t usart1_tx_buff[200];

extern uint8_t usart2_tx_buff[200];

extern uint8_t usart2_buff;
extern uint8_t usart2_sta;

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

extern uint16_t time_ctrl_value_buff[4][8][4];

int usart0_deal(void);
int usart1_deal(void);
void usart0_init(uint16_t ubbr);
void usart1_init(uint16_t ubrr1);
void usart2_init(uint16_t baud);

void usart0_send_char(uint8_t data);
void usart0_send_str(uint8_t *p, uint8_t data_size);
void usart1_send_char(uint8_t data);
void usart1_send_str(uint8_t *p, uint8_t data_size);
void usart2_send_char(uint8_t data);
void usart2_send_str(uint8_t *str, uint8_t data_size);

uint16_t crc_check(uint8_t *p, uint8_t dataSize);

#endif /* UART_H_ */
