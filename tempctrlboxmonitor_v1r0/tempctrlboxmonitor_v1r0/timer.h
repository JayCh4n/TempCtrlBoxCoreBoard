/*
 * timer.h
 *
 * Created: 2018-04-03 09:57:33
 *  Author: chenlong
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "dgus.h"
#include "gpio.h"

#define ALL 0
#define SINGLE 1

extern uint8_t timer2_ovf;
extern uint8_t ctrl_command;
extern uint8_t global;
extern uint8_t all_senser_sta;

void timer0_init();
void timer1_init();
void timer2_init();
//void timer3_init();

#endif /* TIMER_H_ */