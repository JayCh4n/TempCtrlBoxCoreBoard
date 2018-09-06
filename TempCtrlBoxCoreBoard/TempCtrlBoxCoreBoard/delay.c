/*
*delay.c
*
* Created: 2018-06-13 16:27:43
*  Author: chenlong
*/

#include "delay.h"

void _delay_ms(uint32_t nms)
{
	uint32_t ticks;
	
 	ticks = 32 * nms - 17;
	 
 	while (--ticks)
 	{
 		asm("nop");
 	}
	
}

void _delay_us(uint32_t nus)
{
	uint32_t ticks = nus - 5;
	
	while (--ticks)
	{
		asm("nop");
	}
}
