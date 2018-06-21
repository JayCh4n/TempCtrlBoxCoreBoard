/*
*delay.c
*
* Created: 2018-06-13 16:27:43
*  Author: chenlong
*/

#include "delay.h"

void _delay_ms(uint32_t nms)
{
	uint32_t us_ticks;
	us_ticks = nms * 1000;

	while (--us_ticks)
	{
		_delay_1us();
	}
}

void _delay_us(uint32_t nus)
{
	while (--nus)
	{
		_delay_1us();
	}
}

void _delay_1us()
{
	uint8_t ticks;

	ticks = F_CPU / 1000000UL;

	while (--ticks)
	{
		_nop();
	}
}