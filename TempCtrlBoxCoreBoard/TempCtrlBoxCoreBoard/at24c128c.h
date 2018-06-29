/*
 * at24c128c.h
 *
 * Created: 2018-06-23 16:47:40
 *  Author: 123
 */ 


#ifndef AT24C128C_H_
#define AT24C128C_H_

#include "stdio.h"
#include "twi.h"
#include "dgus.h"

/*#define STRUCT_ALIGN_SIZE	4	// 结构体对齐大小 byte*/
#define TEMPLATE_SIZE		sizeof(template_struct_typedef)

#define AT24C128C_ADDR 0x50

extern uint16_t template_eeaddr[44];

uint8_t at24c128c_write_byte(uint16_t addr, uint8_t data);
uint16_t at24c128c_read_byte(uint16_t addr);
void write_sta_to_eeprom(uint8_t template_num, uint8_t sta);
uint8_t read_sta_from_eeprom(uint8_t template_num);
void write_name_to_eeprom(uint8_t template_num, uint32_t name);
uint32_t read_name_from_eeprom(uint8_t template_num);
void write_template_to_eeprom(uint8_t *template_point, uint8_t template_num);
void read_template_from_eeprom(uint8_t *template_point, uint8_t template_num);
void save_preset_to_template(uint8_t template_num);
void template_del(uint8_t template_num);
void set_template_name(uint8_t template_num, uint32_t name);
uint8_t find_template(uint32_t name);
void apply_template(uint8_t template_num);

#endif /* AT24C128C_H_ */