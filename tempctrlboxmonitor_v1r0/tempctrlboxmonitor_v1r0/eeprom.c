/*
 * eeprom.c
 *
 * Created: 2018-04-11 17:30:44
 *  Author: chenlong
 */

#include "eeprom.h"

void read_eeprom_data(void)
{
	uint8_t i = 0;

	if (eeprom_read(FIRST_START_ADDR) != 'y')
	{
		eeprom_write(FIRST_START_ADDR, 'y');

		for (i = 0; i < 12; i++)
		{
			eeprom_write(SINGLE_SETTEMP_EEADDR + (i * 2), set_temp[i]);
			eeprom_write(SINGLE_SETTEMP_EEADDR + (i * 2) + 1, set_temp[i] >> 8);

			eeprom_write(SINGLE_SENSORTYPE_EEADDR + i, sensor_type[i]);
			//			eeprom_write(SINGLE_SWSENSOR_EEADDR+i, switch_sensor[i]);

			eeprom_write(PID_P_EEADDR + (i * 2), p_value[i]);
			eeprom_write(PID_P_EEADDR + (i * 2) + 1, p_value[i] >> 8);

			eeprom_write(PID_I_EEADDR + (i * 2), i_value[i]);
			eeprom_write(PID_I_EEADDR + (i * 2) + 1, i_value[i] >> 8);

			eeprom_write(PID_D_EEADDR + (i * 2), d_value[i]);
			eeprom_write(PID_D_EEADDR + (i * 2) + 1, d_value[i] >> 8);

			eeprom_write(SET_NAME_EEADDR + (i * 4), set_name[i]);
			eeprom_write(SET_NAME_EEADDR + (i * 4) + 1, set_name[i] >> 8);
			eeprom_write(SET_NAME_EEADDR + (i * 4) + 2, set_name[i] >> 16);
			eeprom_write(SET_NAME_EEADDR + (i * 4) + 3, set_name[i] >> 24);
		}
		/*
		for(i=0; i<8; i++)
		{
			eeprom_write(TIME_CTRL_T1_EEADDR+(i*2), t1[i]);
			eeprom_write(TIME_CTRL_T1_EEADDR+(i*2)+1, t1[i]>>8);
			
			eeprom_write(TIME_CTRL_T2_EEADDR+(i*2), t2[i]);
			eeprom_write(TIME_CTRL_T2_EEADDR+(i*2)+1, t2[i]>>8);
			
			eeprom_write(TIME_CTRL_T3_EEADDR+(i*2), t3[i]);
			eeprom_write(TIME_CTRL_T3_EEADDR+(i*2)+1, t3[i]>>8);
			
			eeprom_write(TIME_CTRL_T4_EEADDR+(i*2), t4[i]);
			eeprom_write(TIME_CTRL_T4_EEADDR+(i*2)+1, t4[i]>>8);
		}
		*/
		eeprom_write(PRE_LANGUAGE_EEADDR, 0);
		eeprom_write(ALL_SENSORTYPE_EEADDR, 0);
		eeprom_write(TEMP_UNIT_EEADDR, 0);

		eeprom_write(ALL_SETTEMP_EEADDR, 100);
		eeprom_write(ALL_SETTEMP_EEADDR + 1, 0);

		eeprom_write(PREHEAT_TIME_EEADDR, 3);
	}
	else
	{
		for (i = 0; i < 12; i++)
		{
			set_temp[i] = eeprom_read(SINGLE_SETTEMP_EEADDR + (i * 2) + 1);
			set_temp[i] = (set_temp[i] << 8) |
						  eeprom_read(SINGLE_SETTEMP_EEADDR + (i * 2));
			set_temp_buff[i] = set_temp[i];
			single_set(TEMP, set_temp[i]);

			sensor_type[i] = eeprom_read(SINGLE_SENSORTYPE_EEADDR + i);
			sensor_type_buff[i] = sensor_type[i];
			single_set(SENSOR_TYPE, sensor_type[i]);

			//			switch_sensor[i] = eeprom_read(SINGLE_SWSENSOR_EEADDR+i);
			//			switch_sensor_buff[i] = switch_sensor[i];
			//			single_set(SWITCH_SENSOR, switch_sensor[i]);

			p_value[i] = eeprom_read(PID_P_EEADDR + (i * 2) + 1);
			p_value[i] = (p_value[i] << 8) | eeprom_read(PID_P_EEADDR + (i * 2));

			i_value[i] = eeprom_read(PID_I_EEADDR + (i * 2) + 1);
			i_value[i] = (i_value[i] << 8) | eeprom_read(PID_I_EEADDR + (i * 2));

			d_value[i] = eeprom_read(PID_D_EEADDR + (i * 2) + 1);
			d_value[i] = (d_value[i] << 8) | eeprom_read(PID_D_EEADDR + (i * 2));

			set_name[i] = eeprom_read(SET_NAME_EEADDR + (i * 4) + 3);
			set_name[i] = (set_name[i] << 8) | eeprom_read(SET_NAME_EEADDR + (i * 4) + 2);
			set_name[i] = (set_name[i] << 8) | eeprom_read(SET_NAME_EEADDR + (i * 4) + 1);
			set_name[i] = (set_name[i] << 8) | eeprom_read(SET_NAME_EEADDR + (i * 4));
		}

		/*
		for(i=0; i<8; i++)
		{
			t1[i] = eeprom_read(TIME_CTRL_T1_EEADDR+(i*2)+1);
			t1[i] = (t1[i]<<8) | eeprom_read(TIME_CTRL_T1_EEADDR+(i*2));
			
			t2[i] = eeprom_read(TIME_CTRL_T2_EEADDR+(i*2)+1);
			t2[i] = (t2[i]<<8) | eeprom_read(TIME_CTRL_T2_EEADDR+(i*2));
			
			t3[i] = eeprom_read(TIME_CTRL_T3_EEADDR+(i*2)+1);
			t3[i] = (t3[i]<<8) | eeprom_read(TIME_CTRL_T3_EEADDR+(i*2));
			
			t4[i] = eeprom_read(TIME_CTRL_T4_EEADDR+(i*2)+1);
			t4[i] = (t4[i]<<8) | eeprom_read(TIME_CTRL_T4_EEADDR+(i*2));
		}
		*/

		pre_language = eeprom_read(PRE_LANGUAGE_EEADDR);

		all_sensor_type = eeprom_read(ALL_SENSORTYPE_EEADDR);
		all_sensor_type_buff = all_sensor_type;

		temp_unit = eeprom_read(TEMP_UNIT_EEADDR);
		temp_unit_buff = temp_unit;

		all_temp = eeprom_read(ALL_SETTEMP_EEADDR + 1);
		all_temp = (all_temp << 8) | eeprom_read(ALL_SETTEMP_EEADDR);
		all_temp_buff = all_temp;

		preheat_time = eeprom_read(PREHEAT_TIME_EEADDR);
		preheat_time_buff = preheat_time;
		all_set(PREHEAT_TIME, preheat_time);
	}
}

void eeprom_write(uint16_t addr, uint8_t data)
{
	while (EECR & (1 << EEWE))
		;
	EEAR = addr;
	EEDR = data;

	SREG &= 0x7F; //���ж�
	EECR |= (1 << EEMWE);
	EECR |= (1 << EEWE);
	SREG |= 0x80; //���ж�
}

uint8_t eeprom_read(uint16_t addr)
{
	while (EECR & (1 << EEWE))
		;

	EEAR = addr;
	EECR |= (1 << EERE);
	return EEDR;
}
