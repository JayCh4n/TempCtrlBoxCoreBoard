/*
* dgus.h
*
* Created: 2018-04-04 09:46:55
*  Author: chenlong
*/

#ifndef DGUS_H_
#define DGUS_H_

#include <avr/io.h>
#include "usart.h"
#include "gpio.h"
#include "eeprom.h"
#include "timer.h"

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

//�궨�崮��1��������
#define READ_DATA_ALL 0x01 //USART1��ȡȫ����������
#define PID 0x02
#define TEMP 0x03
#define PREHEAT_TIME 0x04
#define SENSOR_TYPE 0x05
#define SWITCH_SENSOR 0x06
#define READ_SETTING_DATA 0x07
#define SET_FOLLOW 0x08

#define ALL_SET_CMD 0x07 //�˵���ȫ���趨OK����־ ������ѯ���ͻ�ȡ���ư忨����

//�¶ȵ�λ
#define CELSIUS 0x4300
#define FAHRENHEIT 0x0300

//����������
#define TYPE_J 0x4A00
#define TYPE_K 0x0100

//�궨�尴������
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

//����ͨ����
#define CHANNEL0 0x01
#define CHANNEL1 0x02
#define CHANNEL2 0x04
#define CHANNEL3 0x08
#define CHANNEL4 0x10
#define CHANNEL5 0x20
#define CHANNEL6 0x40
#define CHANNEL7 0x80

//������߻�����
#define CHANNEL0_BUFF 0x5A
#define CHANNEL1_BUFF 0x5B

//��ҳ����Ų�λ��ַ
#define MAINPAGE_NUM1_ADDR 0x0000
#define MAINPAGE_NUM2_ADDR 0x0002
#define MAINPAGE_NUM3_ADDR 0x0004
#define MAINPAGE_NUM4_ADDR 0x0006
#define MAINPAGE_NUM5_ADDR 0x0008
#define MAINPAGE_NUM6_ADDR 0x000A

//��ҳ�����������λ��ַ
#define MAIN_OUTRATE1_ADDR 0x000C
#define MAIN_OUTRATE2_ADDR 0x000E
#define MAIN_OUTRATE3_ADDR 0x0010
#define MAIN_OUTRATE4_ADDR 0x0012
#define MAIN_OUTRATE5_ADDR 0x0014
#define MAIN_OUTRATE6_ADDR 0x0016

//��ҳ�������¶Ȳ�λ��ַ
#define MAIN_RUNTEMP1_ADDR 0x0018
#define MAIN_RUNTEMP2_ADDR 0x001A
#define MAIN_RUNTEMP3_ADDR 0x001C
#define MAIN_RUNTEMP4_ADDR 0x001E
#define MAIN_RUNTEMP5_ADDR 0x0020
#define MAIN_RUNTEMP6_ADDR 0x0022

//��ҳ���趨�¶Ȳ�λ��ַ
#define MAIN_SETTEMP1_ADDR 0x0024
#define MAIN_SETTEMP2_ADDR 0x0026
#define MAIN_SETTEMP3_ADDR 0x0028
#define MAIN_SETTEMP4_ADDR 0x002A
#define MAIN_SETTEMP5_ADDR 0x002C
#define MAIN_SETTEMP6_ADDR 0x002E

//��ҳ�洫�������Ͳ�λ��ַ
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

/* �����洫����״̬ͼ����ʾ��  0���ر�  1������  2������  3���澯*/
#define MAIN_STA1_ADDR	0x0270
#define MAIN_STA2_ADDR	0x0274
#define MAIN_STA3_ADDR	0x0278
#define MAIN_STA4_ADDR	0x027C
#define MAIN_STA5_ADDR	0x0280
#define MAIN_STA6_ADDR	0x0284

/* ������澯����ͼ����ʾ�� 2������ 3���ȵ�ż�Ͽ� 4���¶ȹ��� 5�¶ȹ��� 6�ȵ�ż��· 7���ȵ�ż�ӷ� 8�������Ͽ� 9�� ��������·*/
#define MAIN_ALARM1_ADDR	0x0240
#define MAIN_ALARM2_ADDR	0x0248
#define MAIN_ALARM3_ADDR	0x0250
#define MAIN_ALARM4_ADDR	0x0258
#define MAIN_ALARM5_ADDR	0x0260
#define MAIN_ALARM6_ADDR	0x0268

//�澯���������ʾ��λ��ַ
#define ALARM_PAGE_NUM1_ADDR 0x0044
#define ALARM_PAGE_NUM2_ADDR 0x0046
#define ALARM_PAGE_NUM3_ADDR 0x0048
#define ALARM_PAGE_NUM4_ADDR 0x004A
#define ALARM_PAGE_NUM5_ADDR 0x004C
#define ALARM_PAGE_NUM6_ADDR 0x004E
#define ALARM_PAGE_NUM7_ADDR 0x0050

#define ALARM_DEVICE_NUM1_ADDR 0x0078

#define ALAEM_MSG1_ADDR 0x02B0 //02B0-02BF

#define SINGLE_NAME_ADDR 0x0288       //�����趨����������ʾ��ַ��
#define SINGLE_SENSORTYPE_ADDR 0x0290 //�����趨���洫�������͵�ַ
#define SINGLE_NUM_ADDR 0x0030        //�����趨���������ʾ��ַ
#define SINGLE_RUNTEMP_ADDR 0x0032    //�����趨���������¶���ʾ��ַ
#define SINGLE_SETTEMP_ADDR 0x0034    //�����趨�����趨�¶���ʾ��ַ

#define CURVE_PAGE_NUM_ADDR 0x003C        //���߽��������ʾ��ַ
#define CURVE_PAGE_NAME_ADDR 0x0298       //���߽���������ʾ��ַ
#define CURVE_PAGE_RUNTEMP_ADDR 0x003E    //���߽��������¶���ʾ��ַ
#define CURVE_PAGE_SETTEMP_ADDR 0x0040    //���߽����趨�¶���ʾ��ַ
#define CURVE_PAGE_OUTRATE_ADDR 0x0042    //���߽������������ʾ��ַ
#define CURVE_PAGE_SENSORTYPE_ADDR 0x02A0 //���߽��洫����������ʾ��ַ

#define PID_P_ADDR 0x0036 //pid���ý���P������ʾ��ַ
#define PID_I_ADDR 0x0038 //pid���ý���I������ʾ��ַ
#define PID_D_ADDR 0x003A //pid���ý���D������ʾ��ַ

#define MODULE_STATUS_ADDR 0x0070 //�佺����ģ��״̬ͼ����ʾ��ַ    0��ֹͣ		1������
#define MODULE_NUM_ADDR 0x0072    //�佺����ģ�������ʾ��ַ

#define TEMP_UINT_ADDR 0x023C       //�¶ȵ�λ��ʾ��ַ
#define ALL_SENSOR_TYPE_ADDR 0x0294 //ȫ�ִ�����������ʾ��ַ

#define CURVE_TIMELINE_ADDR 0x0052 //����ʱ������ʾ��ַ 2byte ��ʮ�� 0x0052-0x0065

//////////////////
extern uint8_t run_temp_page;
extern uint8_t update_run_temp_flag;
extern uint8_t in_main_page;
extern uint8_t pre_language;
extern uint8_t pre_main_page;
extern uint16_t pre_system_sta;
extern uint8_t set_num;
extern uint8_t set_pid_channel;
extern int8_t curve_page_num;

extern uint16_t p_value[12]; //����
extern uint16_t i_value[12]; //����
extern uint16_t d_value[12]; //����

extern uint8_t output_rate[12];
extern int16_t run_temp[12];
extern int16_t set_temp[12];
extern uint16_t sensor_type[12];
extern uint16_t switch_sensor[12];
extern uint8_t follow_sta[12];
extern uint8_t follow_sta_buff[12];

extern uint16_t preheat_time;
extern uint16_t all_temp;
extern uint16_t all_sensor_type;
extern uint16_t temp_unit;
extern uint32_t set_name[12];

extern uint8_t module_num;

void init_time_ctrl_value(void);
void update_main_page(void);
void update_single_set_page(void);
void update_pid_page(uint8_t channel);
void update_curve_page(void);
void update_time_ctrl_page(void);
void save_time_ctrl_data(void);
void start_time_ctrl(uint8_t slave_num);
void stop_time_ctrl(uint8_t slave_num);
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

#endif /* DGUS_H_ */