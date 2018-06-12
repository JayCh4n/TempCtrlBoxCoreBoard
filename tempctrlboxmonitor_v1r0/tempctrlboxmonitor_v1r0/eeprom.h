/*
 * eeprom.h
 *
 * Created: 2018-04-11 17:31:02
 *  Author: chenlong
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include <avr/io.h>
#include "dgus.h"
#include "usart.h"

#define FIRST_START_ADDR 0x00FC //����Ƿ�Ϊ��һ�����ַ

#define PRE_LANGUAGE_EEADDR 0x0000      //��ǰ��������EEPROM�洢��ַ		0:����		1:Ӣ��
#define ALL_SENSORTYPE_EEADDR 0x0001    //ȫ���趨����������EEPROM�洢��ַ	0:TYPE_J	1:TYPE_K
#define TEMP_UNIT_EEADDR 0x0002         //�¶ȵ�λEEPROM�洢��ַ			0:C			1:F
#define ALL_SETTEMP_EEADDR 0x0003       //ȫ���¶�EEPROM�洢��ַ	 ˫�ֽ� 0x0003 - 0x0004
#define SINGLE_SETTEMP_EEADDR 0x0005    //�����趨�¶�EEPROM��ַ  12��	 ˫�ֽ� 0x0005 - 0x001C
#define SINGLE_SENSORTYPE_EEADDR 0x001D //�����趨����������EEPROME��ַ	12��  ���ֽ�  0x001D - 0x0028
#define PREHEAT_TIME_EEADDR 0x00FE      //Ԥ��ʱ��EEPROM�洢��ַ
#define PID_P_EEADDR 0x002A             //0x002A - 0x0041
#define PID_I_EEADDR 0x0042             //0x0042 - 0x0059
#define PID_D_EEADDR 0x005A             //0x005A - 0x0071
#define SINGLE_SWSENSOR_EEADDR 0x0072   //�����趨�����������ر�EEPROM��ַ 0:�ر� 1:����  ���ֽ�  0x0072 - 0x007D
#define SET_NAME_EEADDR 0x007E          //�趨����EEprome��ַ		0x007E-0x00AD
#define TIME_CTRL_T1_EEADDR 0x00AE
#define TIME_CTRL_T2_EEADDR 0x00BE
#define TIME_CTRL_T3_EEADDR 0x00CE
#define TIME_CTRL_T4_EEADDR 0x00DE

void eeprom_write(uint16_t addr, uint8_t data);
uint8_t eeprom_read(uint16_t addr);
void read_eeprom_data(void);
#endif /* EEPROM_H_ */