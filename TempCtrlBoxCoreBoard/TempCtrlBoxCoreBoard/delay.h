/*
* dalay.h
*
* Created: 2018-06-13 16:21:02
*  Author: chenlong
*/

#ifndef DELAY_MS_
#define DELAY_MS_

#include <avr/io.h>

#define F_CPU 16000000UL
#define _nop() asm volatile("nop")

void _delay_ms(uint32_t nms);
void _delay_us(uint32_t nus);
void _delay_1us();
#endif /* DELAY_MS_ */
