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

#define FIRST_START_ADDR			0x00FC		//检测是否为第一次启动地址

#define PRE_LANGUAGE_EEADDR			0x0000		//当前语言设置EEPROM存储地址		0:中文		1:英文
#define ALL_SENSORTYPE_EEADDR		0x0001		//全局设定传感器类型EEPROM存储地址	0:TYPE_J	1:TYPE_K
#define TEMP_UNIT_EEADDR			0x0002		//温度单位EEPROM存储地址			0:C			1:F
#define ALL_SETTEMP_EEADDR			0x0003		//全局温度EEPROM存储地址	 双字节 0x0003 - 0x0004	
#define SINGLE_SETTEMP_EEADDR		0x0005		//单独设定温度EEPROM地址  12个	 双字节 0x0005 - 0x001C
#define SINGLE_SENSORTYPE_EEADDR	0x001D		//单独设定传感器类型EEPROME地址	12个  单字节  0x001D - 0x0028
#define PREHEAT_TIME_EEADDR			0x00FE		//预热时间EEPROM存储地址		
#define PID_P_EEADDR				0x002A		//0x002A - 0x0041
#define PID_I_EEADDR				0x0042		//0x0042 - 0x0059
#define PID_D_EEADDR				0x005A		//0x005A - 0x0071
#define SINGLE_SWSENSOR_EEADDR		0x0072		//单独设定传感器开启或关闭EEPROM地址 0:关闭 1:开启  单字节  0x0072 - 0x007D
#define SET_NAME_EEADDR				0x007E		//设定名称EEprome地址		0x007E-0x00AD
#define TIME_CTRL_T1_EEADDR			0x00AE
#define TIME_CTRL_T2_EEADDR			0x00BE
#define TIME_CTRL_T3_EEADDR			0x00CE
#define TIME_CTRL_T4_EEADDR			0x00DE


void eeprom_write(uint16_t addr, uint8_t data);
uint8_t eeprom_read(uint16_t addr);
void read_eeprom_data(void);
#endif /* EEPROM_H_ */