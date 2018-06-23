/*
 * at24c128c.h
 *
 * Created: 2018-06-23 16:47:40
 *  Author: 123
 */ 


#ifndef AT24C128C_H_
#define AT24C128C_H_

#include "twi.h"

#define AT24C128C_ADDR 0x50

uint8_t at24c128c_write_byte(uint16_t addr, uint8_t data);
uint16_t at24c128c_read_byte(uint16_t addr);

#endif /* AT24C128C_H_ */