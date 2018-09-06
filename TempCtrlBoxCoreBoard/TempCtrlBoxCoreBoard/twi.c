#include "twi.h"

/*启动条件 0：失败 1：成功*/
uint8_t twi_start()
{
	uint8_t cnt = 0;
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); //发送起始条件
    while (!(TWCR & (1 << TWINT)))
	{
		if(++cnt > 200)
		{
			return 0;
		}	
	}		 //等待TWINT标志设置  这表示START条件已经发送
    if ((TWSR & 0xF8) == START || (TWSR & 0xF8) == REP_START)
        return 1;

    return 0;
}
/* 停止 */
void twi_stop()
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

/*器件寻址 （读写） 0：失败 1：成功*/
uint8_t twi_adrsing(uint8_t addr, uint8_t rw)
{
    uint8_t data;
	uint8_t cnt = 0;
	
    data = (addr << 1) | rw;

    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN); //地址写进TWDR 后加写位   清楚TWINT位来发送地址和读写位

    while (!(TWCR & (1 << TWINT)))
	{
		if(++cnt > 200)
		{
			return 0;
		}	
	} //等待TWINT标志设置  这表示地址加读写位已经发送 并且收到了应答位或者非应答位
    if ((TWSR & 0xF8) == MT_SLA_ACK || (TWSR & 0xF8) == MR_SLA_ACK)
        return 1; //如果状态不是 MT_SLA_ACK 或者 MR_SLA_ACK ，错误

    return 0;
}

/*写字节 0：失败 1：成功*/
uint8_t twi_write_byte(uint8_t data)
{
	uint8_t cnt = 0;
	
    TWDR = data;

    TWCR = (1 << TWINT) | (1 << TWEN); //地址写进TWDR 后加写位   清楚TWINT位来发送地址和读写位

    while (!(TWCR & (1 << TWINT)))
    {
		if(++cnt > 200)
		{
			return 0;
		}	
	} //等待TWINT标志设置  这表示地址加读写位已经发送 并且收到了应答位或者非应答位
    if ((TWSR & 0xF8) == MT_DATA_ACK)
        return 1;

    return 0;
}

//返回值 0-255：数据 0xFFFF：错误  参数：0 非应答 1:应答
uint16_t twi_read_byte(uint8_t ack)
{
	uint8_t cnt = 0;
	
    if (ack)
    {
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
        while(!(TWCR & (1 << TWINT)))
		{
			if(++cnt > 200)
			{
				return 0xFFFF;
			}
		}
        if((TWSR & 0xF8) == MR_DATA_ACK)
            return TWDR;
    }
    else
    {
        TWCR = (1 << TWINT) | (1 << TWEN);
        while(!(TWCR & (1 << TWINT)))
		{
			if(++cnt > 200)
			{
				return 0xFFFF;
			}
		}
        if((TWSR & 0xF8) == MR_DATA_NACK)
            return TWDR;        
    }

    return 0xFFFF;
}

/* twi初始化 不分频 kbs：速率 */
void twi_init(uint16_t kbs)
{
    /* 不分频 */
    TWSR &= ~(1 << TWPS0);
    TWSR &= ~(1 << TWPS1);

    /* 根据速率计算TWBR值 */
    TWBR = (F_CPU / (kbs*1000) - 16) / 2;
}