#ifndef TWI_H_
#define TWI_H_

#include <avr/io.h>
#include "delay.h"

#define AT24C128C_ADDR 0x50
#define READ    1
#define WRITE   0

#define ACK 1
#define NACK 0

#define SCL_PIN_OUT     DDRD |= (1 << 0)
#define SDA_PIN_OUT     DDRD |= (1 << 1)
#define SET_SCL_HIGH    PORTD |= (1<<0)
#define SET_SCL_LOW     PORTD &= ~(1<<0)
#define SET_SDA_HIGH    PORTD |= (1<<1)
#define SET_SDA_LOW     PORTD &= ~(1<<1)

#define START 0x80
#define REP_START 0x10
#define ARB_LOST 0x38

#define MT_SLA_ACK 0x18
#define MT_SLA_NACK 0x20
#define MT_DATA_ACK 0x28
#define MT_DATA_NACK 0x30

#define MR_SLA_ACK 0X40
#define MR_SLA_NACK 0X48
#define MR_DATA_ACK 0X50
#define MR_DATA_NACK 0X58

void twi_init(uint16_t kbs);
uint8_t twi_start();
void twi_stop();
uint8_t twi_adrsing(uint8_t addr, uint8_t rw);
uint8_t twi_write_byte(uint8_t data);
uint16_t twi_read_byte(uint8_t ack);
void at24c128c_write_byte(uint16_t addr, uint8_t data);
uint8_t at24c128c_read_byte(uint16_t addr);

#endif /* TWI_H_ */