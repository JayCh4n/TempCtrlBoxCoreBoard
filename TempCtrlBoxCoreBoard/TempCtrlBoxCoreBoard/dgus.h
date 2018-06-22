/*
* dgus.h
*
* Created: 2018-04-04 09:46:55
*  Author: chenlong
*/

#ifndef DGUS_H_
#define DGUS_H_

#include <avr/io.h>
#include <avr/eeprom.h>
#include "usart.h"
#include "gpio.h"
#include "timer.h"

/*最大主页面数和每页能显示多少通道 可根据这两个宏定义确定通道数*/
#define MAX_PAGE_QUANTITY			4
#define IQR_QUANTITY_PER_PAGE		6
#define MAX_IQR_QUANTITY			(MAX_PAGE_QUANTITY * IQR_QUANTITY_PER_PAGE)
#define TEMP_CTRL_BOARD_QUANTITY	(MAX_IQR_QUANTITY / 4)

#define EN_INTERRUPT SREG |= 0x80
#define DISEN_INTERRUPT SREG &= 0x7F

#define OPEN_CURVE(count)   \
	do                      \
	{                       \
		timer2_ovf = count; \
		TCNT2 = 0xB2;       \
		TCCR2 |= 0x05;      \
	} while (0)
#define CLOSE_CURVE TCCR2 &= 0xF8

//宏定义串口1发送命令
#define READ_DATA_ALL 0x01 //USART1读取全部数据命令
#define PID 0x02
#define TEMP 0x03
#define PREHEAT_TIME 0x04
#define SENSOR_TYPE 0x05
#define SWITCH_SENSOR 0x06
#define READ_SETTING_DATA 0x07
#define SET_FOLLOW 0x08

#define ALL_SET_CMD 0x07 //菜单里全局设定OK键标志 用来轮询发送获取控制板卡数据

//温度单位
#define CELSIUS 0x4300
#define FAHRENHEIT 0x0300

//传感器类型
#define TYPE_J 0x4A00
#define TYPE_K 0x0100

//宏定义按键动作
#define SWITCH_LANGUAGE 0x0000
#define CLEAR_ALARM_SOUND 0x0001
#define MAIN_PAGE_UP 0x0002
#define MAIN_PAGE_DOWN 0x0003
#define MAIN_SENSOR1_SET 0x0004
#define MAIN_SENSOR2_SET 0x0005
#define MAIN_SENSOR3_SET 0x0006
#define MAIN_SENSOR4_SET 0x0007
#define MAIN_SENSOR5_SET 0x0008
#define MAIN_SENSOR6_SET 0x0009
#define SINGLE_SET_OK 0x000A
#define ALL_SET_OK 0x000B
#define PID_SET_OK 0x000C
#define CURVE_ZOOM_OUT 0x000D
#define CURVE_ZOOM_IN 0x000E
#define CURVE_PAGE_UP 0x000F
#define CURVE_PAGE_DOWN 0x0010
#define CLEAR_ALARM_MSG 0x0011
#define ALARM_PAGE_UP 0x0012
#define ALARM_PAGE_DOWN 0x0013
#define SINGLE_SET_BACK 0x0014
#define ALL_SET_BACK 0x0015
#define PID_PAGE_ENTER 0x0016
#define ALARM_PAGE_ENTER 0x0017
#define CURVE_PAGE_ENTER 0x0018
#define CURVE_PAGE_BACK 0x0019
#define MENU_PAGE_ENTER 0x001A
#define TIME_CTRL_ENTER 0x001B
#define TIME_CTRL_STOP 0x001C
#define TIME_CTRL_START 0x001D
#define TIME_CTRL_SAVEDAT 0x001E
#define TIME_CTRL_PAGEUP 0x001F
#define TIME_CTRL_PAGEDOWN 0x0020
#define IQR1_TEST 0x0021
#define IQR2_TEST 0x0022
#define IQR3_TEST 0x0023
#define IQR4_TEST 0x0024
#define IQR5_TEST 0x0025
#define IQR6_TEST 0x0026
#define IQR7_TEST 0x0027
#define IQR8_TEST 0x0028
#define PID_PAGE_UP	0x0029
#define PID_PAGE_DOWN 0x002A

//曲线通道号
#define CHANNEL0 0x01
#define CHANNEL1 0x02
#define CHANNEL2 0x04
#define CHANNEL3 0x08
#define CHANNEL4 0x10
#define CHANNEL5 0x20
#define CHANNEL6 0x40
#define CHANNEL7 0x80

//清除曲线缓冲区
#define CHANNEL0_BUFF 0x5A
#define CHANNEL1_BUFF 0x5B

//主页面序号槽位地址
#define MAINPAGE_NUM1_ADDR 0x0000
#define MAINPAGE_NUM2_ADDR 0x0002
#define MAINPAGE_NUM3_ADDR 0x0004
#define MAINPAGE_NUM4_ADDR 0x0006
#define MAINPAGE_NUM5_ADDR 0x0008
#define MAINPAGE_NUM6_ADDR 0x000A

//主页面输出比例槽位地址
#define MAIN_OUTRATE1_ADDR 0x000C
#define MAIN_OUTRATE2_ADDR 0x000E
#define MAIN_OUTRATE3_ADDR 0x0010
#define MAIN_OUTRATE4_ADDR 0x0012
#define MAIN_OUTRATE5_ADDR 0x0014
#define MAIN_OUTRATE6_ADDR 0x0016

//主页面运行温度槽位地址
#define MAIN_RUNTEMP1_ADDR 0x0018
#define MAIN_RUNTEMP2_ADDR 0x001A
#define MAIN_RUNTEMP3_ADDR 0x001C
#define MAIN_RUNTEMP4_ADDR 0x001E
#define MAIN_RUNTEMP5_ADDR 0x0020
#define MAIN_RUNTEMP6_ADDR 0x0022

//主页面设定温度槽位地址
#define MAIN_SETTEMP1_ADDR 0x0024
#define MAIN_SETTEMP2_ADDR 0x0026
#define MAIN_SETTEMP3_ADDR 0x0028
#define MAIN_SETTEMP4_ADDR 0x002A
#define MAIN_SETTEMP5_ADDR 0x002C
#define MAIN_SETTEMP6_ADDR 0x002E

//主页面传感器类型槽位地址
#define MAIN_SENSOR1_TYPE_ADDR 0x0230
#define MAIN_SENSOR2_TYPE_ADDR 0x0232
#define MAIN_SENSOR3_TYPE_ADDR 0x0234
#define MAIN_SENSOR4_TYPE_ADDR 0x0236
#define MAIN_SENSOR5_TYPE_ADDR 0x0238
#define MAIN_SENSOR6_TYPE_ADDR 0x023A

#define MAIN_NAME1_ADDR 0x0200
#define MAIN_NAME2_ADDR 0x0208
#define MAIN_NAME3_ADDR 0x0210
#define MAIN_NAME4_ADDR 0x0218
#define MAIN_NAME5_ADDR 0x0220
#define MAIN_NAME6_ADDR 0x0228

/* 主界面传感器状态图标显示槽  0：关闭  1：开启  2：跟随  3：告警*/
#define MAIN_STA1_ADDR 0x0270
#define MAIN_STA2_ADDR 0x0274
#define MAIN_STA3_ADDR 0x0278
#define MAIN_STA4_ADDR 0x027C
#define MAIN_STA5_ADDR 0x0280
#define MAIN_STA6_ADDR 0x0284

/* 主界面告警类型图标显示槽 2：正常 3：热电偶断开 4：温度过高 5温度过低 6热电偶短路 7：热电偶接反 8加热器断开 9： 加热器短路*/
#define MAIN_ALARM1_ADDR 0x0240
#define MAIN_ALARM2_ADDR 0x0248
#define MAIN_ALARM3_ADDR 0x0250
#define MAIN_ALARM4_ADDR 0x0258
#define MAIN_ALARM5_ADDR 0x0260
#define MAIN_ALARM6_ADDR 0x0268

//告警界面序号显示槽位地址
#define ALARM_PAGE_NUM1_ADDR 0x0044
#define ALARM_PAGE_NUM2_ADDR 0x0046
#define ALARM_PAGE_NUM3_ADDR 0x0048
#define ALARM_PAGE_NUM4_ADDR 0x004A
#define ALARM_PAGE_NUM5_ADDR 0x004C
#define ALARM_PAGE_NUM6_ADDR 0x004E
#define ALARM_PAGE_NUM7_ADDR 0x0050

#define ALARM_DEVICE_NUM1_ADDR 0x0078

#define ALAEM_MSG1_ADDR 0x02B0 //02B0-02BF

#define SINGLE_NAME_ADDR 0x0288		  //单独设定界面名称显示地址低
#define SINGLE_SENSORTYPE_ADDR 0x0290 //单独设定界面传感器类型地址
#define SINGLE_NUM_ADDR 0x0030		  //单独设定界面序号显示地址
#define SINGLE_RUNTEMP_ADDR 0x0032	//单独设定界面运行温度显示地址
#define SINGLE_SETTEMP_ADDR 0x0034	//单独设定界面设定温度显示地址

#define CURVE_PAGE_NUM_ADDR 0x003C		  //曲线界面序号显示地址
#define CURVE_PAGE_NAME_ADDR 0x0298		  //曲线界面名称显示地址
#define CURVE_PAGE_RUNTEMP_ADDR 0x003E	//曲线界面运行温度显示地址
#define CURVE_PAGE_SETTEMP_ADDR 0x0040	//曲线界面设定温度显示地址
#define CURVE_PAGE_OUTRATE_ADDR 0x0042	//曲线界面输出比例显示地址
#define CURVE_PAGE_SENSORTYPE_ADDR 0x02A0 //曲线界面传感器类型显示地址

#define PID_P_ADDR 0x0036 //pid设置界面P参数显示地址
#define PID_I_ADDR 0x0038 //pid设置界面I参数显示地址
#define PID_D_ADDR 0x003A //pid设置界面D参数显示地址

#define MODULE_STATUS_ADDR 0x0070 //射胶控制模块状态图标显示地址    0：停止		1：开启
#define MODULE_NUM_ADDR 0x0072	//射胶控制模块号码显示地址

#define TEMP_UINT_ADDR 0x023C		//温度单位显示地址
#define ALL_SENSOR_TYPE_ADDR 0x0294 //全局传感器类型显示地址

#define CURVE_TIMELINE_ADDR 0x0052 //曲线时间轴显示地址 2byte 共十个 0x0052-0x0065

#define MAX_ALARM_HISTORY 70

#define FIRST_START_ADDR 0x0000 //检测是否为第一次启动地址

#define PRE_LANGUAGE_EEADDR 0x0001 //当前语言设置EEPROM存储地址		0:中文		1:英文
// #define ALL_SENSORTYPE_EEADDR 0x0001    //全局设定传感器类型EEPROM存储地址	0:TYPE_J	1:TYPE_K
// #define TEMP_UNIT_EEADDR 0x0002         //温度单位EEPROM存储地址			0:C			1:F
// #define ALL_SETTEMP_EEADDR 0x0003       //全局温度EEPROM存储地址	 双字节 0x0003 - 0x0004
// #define SINGLE_SETTEMP_EEADDR 0x0005    //单独设定温度EEPROM地址  12个	 双字节 0x0005 - 0x001C
// #define SINGLE_SENSORTYPE_EEADDR 0x001D //单独设定传感器类型EEPROME地址	12个  单字节  0x001D - 0x0028
// #define PREHEAT_TIME_EEADDR 0x00FE      //预热时间EEPROM存储地址
// #define PID_P_EEADDR 0x002A             //0x002A - 0x0041
// #define PID_I_EEADDR 0x0042             //0x0042 - 0x0059
// #define PID_D_EEADDR 0x005A             //0x005A - 0x0071
// #define SINGLE_SWSENSOR_EEADDR 0x0072   //单独设定传感器开启或关闭EEPROM地址 0:关闭 1:开启  单字节  0x0072 - 0x007D
// #define SET_NAME_EEADDR 0x007E          //设定名称EEprome地址		0x007E-0x00AD
// #define TIME_CTRL_T1_EEADDR 0x00AE
// #define TIME_CTRL_T2_EEADDR 0x00BE
// #define TIME_CTRL_T3_EEADDR 0x00CE
// #define TIME_CTRL_T4_EEADDR 0x00DE
#define ALARM_CNT_EEADDR 0x0002		  //告警数量
#define ALARM_HISTORY_EEADDR 0x0003   //告警记录EEPROM地址 双字节 高：告警类型 低：告警设备号0～12 共70条记录 占140字节 0x0003~0x8E
#define TIME_CTRL_VALUE_EEADDR 0x008F //射胶阀控制时间参数EEPROM地址 双字节 共256bytes  0x008F ~ 0x018E

typedef enum
{
	thermocouple_disconnected,
	temp_too_high,
	temp_too_low,
	thermocouple_short_circuit,
	thermocouple_reversed,
	heater_disconnected,
	heater_short_circuit
} alarm_typedef;

typedef struct
{
	alarm_typedef alarm_type;
	uint8_t alarm_device_num;
} alarm_struct_typedef;

//////////////////
extern uint8_t run_temp_page;
extern uint8_t update_run_temp_flag;
extern uint8_t in_main_page;
extern uint8_t pre_language;
extern int8_t pre_main_page;
extern uint16_t pre_system_sta;
extern uint8_t set_num;
extern uint8_t set_pid_channel;
extern int8_t curve_page_num;

extern uint16_t p_value[MAX_IQR_QUANTITY]; //发送
extern uint16_t i_value[MAX_IQR_QUANTITY]; //发送
extern uint16_t d_value[MAX_IQR_QUANTITY]; //发送

extern uint8_t output_rate[MAX_IQR_QUANTITY];
extern int16_t run_temp[MAX_IQR_QUANTITY];
extern int16_t set_temp[MAX_IQR_QUANTITY];
extern uint16_t sensor_type[MAX_IQR_QUANTITY];
extern uint16_t switch_sensor[MAX_IQR_QUANTITY];
extern uint8_t follow_sta[MAX_IQR_QUANTITY];
extern uint8_t follow_sta_buff[MAX_IQR_QUANTITY];

extern uint8_t sensor_sta[MAX_IQR_QUANTITY];
extern uint8_t pre_sensor_sta[MAX_IQR_QUANTITY];

extern uint16_t preheat_time;
extern uint16_t all_temp;
extern uint16_t all_sensor_type;
extern uint16_t temp_unit;
extern uint32_t set_name[MAX_IQR_QUANTITY];

extern uint8_t module_num;
extern uint16_t time_ctrl_value[4][8][4];

extern uint8_t alarm_cnt;
extern alarm_struct_typedef alarm_history[MAX_ALARM_HISTORY];

// void init_time_ctrl_value(void);
void init_variable(void);
void update_main_page(void);
void update_single_set_page(void);
void update_pid_page(uint8_t channel);
void update_curve_page(void);
void update_time_ctrl_page(void);
void save_time_ctrl_data(void);
void start_time_ctrl(uint8_t slave_num);
void stop_time_ctrl(uint8_t slave_num);
void time_ctrl_test(uint8_t slave_num, uint8_t command);
void switch_language(void);
void change_page(uint8_t page_num);
void key_action(uint16_t key_code);
void send_variables(uint16_t addr, uint16_t data);
void send_name(uint16_t addr, uint32_t name);
void send_to_channel(uint8_t ch, uint16_t value);
void send_alarm_msg(uint8_t type, uint8_t msg_num, uint16_t sen_num);
void send_request_all(uint8_t addr);
void get_data_all(void);
void clear_curve_buff(uint8_t channel);

void single_set_ok(void);
void all_set_ok(void);
void pid_set_ok(void);
void clear_alarm_msg(uint8_t msg_num);
void clear_all_alarm_msg(void);
void update_alarm_page(uint8_t page_num);
void single_set_back(void);
void all_set_back(void);

void switch_all_sensor(uint16_t sta);
void get_set_name(void);
void single_set(uint8_t command, uint16_t value);
void all_set(uint8_t command, uint16_t value);
void set_pid(void);
uint8_t read_setting_data(uint8_t addr);
void read_setting_data_all(void);
void get_setting_data(uint8_t addr);
void alarm_monitor(void);

#endif /* DGUS_H_ */