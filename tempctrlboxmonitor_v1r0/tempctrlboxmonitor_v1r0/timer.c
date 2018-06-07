/*
 * timer.c
 *
 * Created: 2018-04-03 09:57:22
 *  Author: chenlong
 */ 
#include "timer.h"

uint8_t timer2_ovf = 0;
uint8_t ctrl_command = READ_DATA_ALL;		//����
uint8_t global;								//�Ƿ�Ϊȫ���趨
uint8_t all_senser_sta;

ISR (TIMER0_OVF_vect)
{
	static uint8_t usart1_cnt = 0;						//USART1����ʱ�����
	static uint8_t pre_usart1_rx_cnt = 0;				//USART1�ϴν��ռ���
	
	static uint8_t usart0_cnt = 0;						//USART0����ʱ�����
	static uint8_t pre_usart0_rx_cnt = 0;				//USART0�ϴν��ռ���
	
	static uint8_t time_cnt = 0;						//ʱ�����
	static uint8_t slave_num = 1;						//���ư����
	static uint16_t update_run_temp_cnt = 0;
	
	EN_INTERRUPT;
	TCNT0 = 0xB2;
	
	time_cnt++;
	
	if(time_cnt >= 100)
	{
		time_cnt = 0;
		
		switch(ctrl_command)
		{
			case READ_DATA_ALL: send_request_all(slave_num++); if(slave_num >= 4) slave_num = 1; break;
			
			case PID:			set_pid(); break;
			
			case TEMP:			single_set(TEMP, set_temp[set_num]); break;
			
			case SENSOR_TYPE:   single_set(SENSOR_TYPE, sensor_type[set_num]); break;
								
			case SWITCH_SENSOR: if(global == SINGLE)
								{
									single_set(SWITCH_SENSOR, switch_sensor[set_num]);
								}
								else
								{
									 switch_all_sensor(all_senser_sta);
								}
								break;
								
			case ALL_SET_CMD:	all_set_ok(); break;
			
			default: break;
		}

	}													//ÿһ�ٺ�������һ�����ذ巢�����вɼ�����  300�������3�����ذ�ɼ����ݵķ���

	if(update_run_temp_flag)							//����ڵ����趨���棬1s����һ�������¶�
	{
		update_run_temp_cnt++;
		
		if(update_run_temp_cnt >= 200)
		{
			update_run_temp_cnt = 0;
			
			if(run_temp_page)
			{
				send_variables(SINGLE_RUNTEMP_ADDR, run_temp[set_num]+temp_unit*((run_temp[set_num]*8/10)+32));		//�����趨������������¶�
			}
			else
			{
				send_variables(CURVE_PAGE_RUNTEMP_ADDR, run_temp[curve_page_num] +
														temp_unit*(run_temp[curve_page_num]*8/10 + 32));	//���߽�����������¶�
														
				send_variables(CURVE_PAGE_OUTRATE_ADDR, output_rate[curve_page_num]);						//���߽�������������
			}
		}
	}
	
	
	if(usart1_rx_cnt != pre_usart1_rx_cnt)
	{
		pre_usart1_rx_cnt = usart1_rx_cnt;
		usart1_cnt = 0;
	}													//���usart1��ǰ���ռ������ϴμ�����һ�� ��ʾ�����ݽ���  ���ʱ�����
	else												
	{		
		if(usart1_rx_cnt != 0)							//���usart1���ռ���Ϊ0 ��Ϊû�п�ʼ��������  �򲻽��г�ʱ����
		{
			usart1_cnt++;
			
			if(usart1_cnt >= 4)
			{
				usart1_rx_lenth = usart1_rx_cnt;
				usart1_rx_end = 1;
				
				usart1_rx_cnt = 0;
				pre_usart1_rx_cnt = 0;
				
				usart1_cnt = 0;		
			}
		}
	}													//�������21����û�����ݽ���  ��Ϊ���һ�����ݵĽ���      ���ʱ�����
	
	if(usart0_rx_cnt != pre_usart0_rx_cnt)
	{
		pre_usart0_rx_cnt = usart0_rx_cnt;
		usart0_cnt = 0;
	}													//���usart0��ǰ���ռ������ϴμ�����һ�� ��ʾ�����ݽ���  ���ʱ�����
	else
	{
		if(usart0_rx_cnt != 0)							//���usart0���ռ���Ϊ0   ��Ϊû�п�ʼ��������  �򲻽��г�ʱ����
		{
			usart0_cnt++;
			
			if(usart0_cnt >= 4)
			{
				usart0_rx_lenth = usart0_rx_cnt;
				usart0_rx_end = 1;
				
				usart0_rx_cnt = 0;
				pre_usart0_rx_cnt = 0;
				
				usart0_cnt = 0;
			}
		}
	}													//�������21����û�����ݽ���  ��Ϊ���һ�����ݵĽ���      ���ʱ�����

}

ISR(TIMER1_COMPA_vect)
{
	uint8_t i = 0;

	static uint16_t time_cnt[8] = {0};
	static uint8_t sta[8] = {0};
	static uint8_t pre_sta[8] = {0};
	static uint8_t curve_cnt = 0;
	
	EN_INTERRUPT;

	for(i=0; i<8; i++)
	{
		if(first_start_iqr[i] == 1)
		{
			first_start_iqr[i] = 0;
			time_cnt[i] = t1[i];
			
			sta[i] = 0;
			pre_sta[i] = 0;
			clear_curve_buff(CHANNEL0_BUFF);
		}
		else if(pre_sta[i] != sta[i])
		{
			switch(sta[i])
			{
				case 0: time_cnt[i] = t1[i]; break;
				case 1:	time_cnt[i] = t2[i]; break;
				case 2: time_cnt[i] = t3[i]; break;
				case 3: time_cnt[i] = t4[i]; break;
				default: break;
			}
			
			pre_sta[i] = sta[i];
		}
		
		if(start_iqr[i] == 1)
		{
			time_cnt[i]--;
			if(time_cnt[i] <= 0)
			{
				if((sta[i] == 0) || (sta[i] == 2))
				{
					PORTC &= ~(1<<i);	//����ͨ��
				}
				else if((sta[i] == 1) || (sta[i] == 3))
				{
					PORTC |= (1<<i);	//�ر�ͨ��
				}
				
				sta[i] = sta[i] + 1;
				if(sta[i] >= 4)
				{
					sta[i] = 0;
				}
			}
			
			switch(i)
			{
				case 0: LED4_ON; break;
				case 1: LED3_ON; break;
				case 2: LED2_ON; break;
				case 3: LED1_ON; break;
			}
		}
		else if(start_iqr[i] == 0)
		{
			PORTC |= (1<<i);
			
			switch(i)
			{
				case 0: LED4_OFF; break;
				case 1: LED3_OFF; break;
				case 2: LED2_OFF; break;
				case 3: LED1_OFF; break;
			}

		}
	}
	
	curve_cnt++;
	if(curve_cnt >= 25)
	{
		curve_cnt = 0;
		
		if(in_time_ctrl_page == 1)
		{
			if(start_iqr[pre_iqr] == 1)
			{
				if((sta[pre_iqr] == 0) || (sta[pre_iqr] == 2))
				{
					send_to_channel(CHANNEL0, 0x11);
				}
				else if((sta[pre_iqr] == 1) || (sta[pre_iqr] == 3))
				{
					send_to_channel(CHANNEL0, 0x33);
				}
			}
		}
	}
	
}

ISR(TIMER2_OVF_vect)
{
	static uint8_t time_cnt = 0;
	int16_t temp_differ = 0;
	
	EN_INTERRUPT;
	TCNT2 = 0xB2;
	
	temp_differ = (run_temp[curve_page_num] - set_temp[curve_page_num])+100;
	
	time_cnt++;
	if(time_cnt >= timer2_ovf)
	{
		time_cnt = 0;
		send_to_channel(CHANNEL0, temp_differ);
	}
	
}

void timer0_init()
{
		TCCR0 &= 0xF8;		//�رն�ʱ��0
		
		TCNT0 = 0xB2;		//��ʱ5ms
		
		TIMSK &= 0xFC;
		TIMSK |= 0x01;		//ʹ�ܶ�ʱ��0����ж�
		
		TCCR0 |= 0x07;		//������ʱ��0 ����ģʽTOP:0xFF, OC0������ź�, clk/1024
}

void timer1_init()
{
	TCCR1B |= 0x08;			//WGM12λ��1			
	TCCR1B &= 0xF8;			//�رն�ʱ�������ʱ��ѡ��λ CS10 CS11 CS12��
	
	OCR1AH = 0x09;			//��ʱ10ms
	OCR1AL = 0xC4;
	
	TIMSK |= 0x10;			//��ʱ��1 A����Ƚ��ж�ʹ��
	
	TCCR1B |= 0x03;			//������ʱ��1 CTCģʽ top:OCR1A, OC1A OC1B OC1C DISCONNECT,CLK/64
}

void timer2_init()
{
	TCCR2 &= 0xF8;			//�رն�ʱ��
	
	TCNT2 = 0xB2;			//��ʱ5ms
	
	TIMSK &= 0x3F;			
	TIMSK |= 0x40;			//ʹ�ܶ�ʱ��2����ж�
	
	TCCR2 |= 0x05;			//������ʱ��2 ����ģʽTOP:0xFF, OC0������ź�, clk/1024
	TCCR2 &= 0xF8;
}
