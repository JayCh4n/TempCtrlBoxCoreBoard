#include "twi.h"

// uint8_t write_byte(uint8_t addr, uint8_t data)
// {
//     TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); //发送起始条件
//     while (!(TWCR & (1 << TWINT)))
//         ; //等待TWINT标志设置  这表示START条件已经发送

//     if ((TWSR & 0xF8) != START)
//         return 0; //检查TWI状态寄存器的值  掩码预分频位  如果与START不同的状态变为ERROR

//     TWDR = addr;
//     TWCR = (1 << TWINT) | (1 << TWEN); //地址写进TWDR 后加写位   清楚TWINT位来发送地址和读写位

//     while (!(TWCR & (1 << TWINT)))
//         ; //等待TWINT标志设置  这表示地址加读写位已经发送 并且收到了应答位或者非应答位

//     if ((TWSR & 0xF8) != MT_SLA_ACK)
//         return 0; //如果状态不是MT_SLA_ACK 错误
//     TWDR = data;
//     TWCR = (1 << TWINT) | (1 << TWEN); //数据写进TWDR，清楚TWINT来发送数据

//     while (!(TWCR & (1 << TWINT)))
//         ; //等待TWINT标志设置  这表示数据已经发送 并且收到了应答位或非应答位

//     if ((TWSR & 0xF8) != MT_DATA_ACK)
//         return 0; //如果状态不是MT_DATA_ACK 错误

//     TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO); //发送停止位

//     return 1;
// }

void at24c128c_write_byte(uint16_t addr, uint8_t data)
{
    uint8_t addr_h, addr_l;

    addr_h = addr >> 8;
    addr_l = addr;

    twi_start();
    twi_adrsing(AT24C128C_ADDR, WRITE);

    twi_write_byte(addr_h);
    twi_write_byte(addr_l);

    twi_write_byte(data);
    
    twi_stop();
}

uint8_t at24c128c_read_byte(uint16_t addr)
{
    uint8_t data;
    uint8_t addr_h, addr_l;

    addr_h = addr >> 8;
    addr_l = addr;

    twi_start();
    twi_adrsing(AT24C128C_ADDR, WRITE);

    twi_write_byte(addr_h);
    twi_write_byte(addr_l);

    twi_start();
    twi_adrsing(AT24C128C_ADDR, READ);

    data = twi_read_byte(NACK);

    twi_stop();  

    return data;
}

uint8_t twi_start()
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); //发送起始条件
    while (!(TWCR & (1 << TWINT)))
        ; //等待TWINT标志设置  这表示START条件已经发送
    if ((TWSR & 0xF8) == START)
        return 1;

    return 0;
}

void twi_stop()
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

uint8_t twi_adrsing(uint8_t addr, uint8_t rw)
{
    uint8_t data;

    data = (addr << 1) | rw;

    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN); //地址写进TWDR 后加写位   清楚TWINT位来发送地址和读写位

    while (!(TWCR & (1 << TWINT)))
        ; //等待TWINT标志设置  这表示地址加读写位已经发送 并且收到了应答位或者非应答位
    if ((TWSR & 0xF8) == MT_SLA_ACK || (TWSR & 0xF8) = MR_SLA_ACK)
        return 1; //如果状态不是 MT_SLA_ACK 或者 MR_SLA_ACK ，错误

    return 0;
}

uint8_t twi_write_byte(uint8_t data)
{
    TWDR = data;

    TWCR = (1 << TWINT) | (1 << TWEN); //地址写进TWDR 后加写位   清楚TWINT位来发送地址和读写位

    while (!(TWCR & (1 << TWINT)))
        ; //等待TWINT标志设置  这表示地址加读写位已经发送 并且收到了应答位或者非应答位
    if ((TWSR & 0xF8) == MT_DATA_ACK)
        return 1;

    return 0;
}

//返回值 0-255：数据 0xFFFF：错误  参数：0 非应答 1:应答
uint16_t twi_read_byte(uint8_t ack)
{
    if (ack)
    {
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
        while(!(TWCR & (1 << TWINT)));
        if((TWSR & 0xF8) == MR_DATA_ACK)
            return TWDR;
    }
    else
    {
        TWCR = (1 << TWINT) | (1 << TWEN);
        while(!(TWCR & (1 << TWINT)));
        if((TWSR & 0xF8) == MR_DATA_NACK)
            return TWDR;        
    }

    return 0xFFFF;
}

void twi_init(uint16_t kbs)
{
    SCL_PIN_OUT;
    SDA_PIN_OUT;

    SET_SCL_HIGH;
    SET_SDA_HIGH;

    /* 不分频 */
    TWSR &= ~(1 << TWPS0);
    TWSR &= ~(1 << TWPS1);

    /* 根据速率计算TWBR值 */
    TWBR = (F_CPU - kbs * 16) / kbs / 2;
}