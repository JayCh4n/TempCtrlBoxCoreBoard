/*
* gpio.h
*
* Created: 2018-04-04 12:59:38
*  Author: chenlong
*/

#ifndef GPIO_H_
#define GPIO_H_

//定义LED灯端口方向
#define LED1_PIN_OUTPUT DDRA |= (1 << 6)
#define LED2_PIN_OUTPUT DDRA |= (1 << 5)
#define LED3_PIN_OUTPUT DDRA |= (1 << 4)
#define LED4_PIN_OUTPUT DDRA |= (1 << 3)
#define LED5_PIN_OUTPUT DDRA |= (1 << 2)
#define LED6_PIN_OUTPUT DDRA |= (1 << 1)
#define LED7_PIN_OUTPUT DDRA |= (1 << 0)

//LED开启宏定义
#define LED1_OFF PORTA |= (1 << 6)
#define LED2_OFF PORTA |= (1 << 5)
#define LED3_OFF PORTA |= (1 << 4)
#define LED4_OFF PORTA |= (1 << 3)
#define LED5_OFF PORTA |= (1 << 2)
#define LED6_OFF PORTA |= (1 << 1)
#define LED7_OFF PORTA |= (1 << 0)

//LED关闭宏定义
#define LED1_ON PORTA &= ~(1 << 6)
#define LED2_ON PORTA &= ~(1 << 5)
#define LED3_ON PORTA &= ~(1 << 4)
#define LED4_ON PORTA &= ~(1 << 3)
#define LED5_ON PORTA &= ~(1 << 2)
#define LED6_ON PORTA &= ~(1 << 1)
#define LED7_ON PORTA &= ~(1 << 0)

/*
//定义射胶阀控制端口方向
#define IQR0_PIN_OUTPUT			DDRC |= (1<<0)
#define IQR1_PIN_OUTPUT			DDRC |= (1<<1)
#define IQR2_PIN_OUTPUT			DDRC |= (1<<4)
#define IQR3_PIN_OUTPUT			DDRC |= (1<<5)
#define IQR4_PIN_OUTPUT			DDRC |= (1<<2)
#define IQR5_PIN_OUTPUT			DDRC |= (1<<3)
#define IQR6_PIN_OUTPUT			DDRC |= (1<<6)
#define IQR7_PIN_OUTPUT			DDRC |= (1<<7)

//射胶阀通道开启宏定义
#define IQR0_OPEN				PORTC &= ~(1<<0)
#define IQR1_OPEN				PORTC &= ~(1<<1)
#define IQR2_OPEN				PORTC &= ~(1<<4)
#define IQR3_OPEN				PORTC &= ~(1<<5)
#define IQR4_OPEN				PORTC &= ~(1<<2)
#define IQR5_OPEN				PORTC &= ~(1<<3)
#define IQR6_OPEN				PORTC &= ~(1<<6)
#define IQR7_OPEN				PORTC &= ~(1<<7)


//射胶阀通道关闭宏定义
#define IQR0_CLOSE				PORTC |= (1<<0)
#define IQR1_CLOSE				PORTC |= (1<<1)
#define IQR2_CLOSE				PORTC |= (1<<4)
#define IQR3_CLOSE				PORTC |= (1<<5)
#define IQR4_CLOSE				PORTC |= (1<<2)
#define IQR5_CLOSE				PORTC |= (1<<3)
#define IQR6_CLOSE				PORTC |= (1<<6)
#define IQR7_CLOSE				PORTC |= (1<<7)
*/

//定义继电器控制端口方向
#define ALARM_PIN_OUTPUT DDRD |= (1 << 4)
#define ACCON_PIN_OUTPUT DDRD |= (1 << 5)

//继电器开启宏定义
#define ALARM_ON PORTD |= (1 << 4)
#define ACCON_ON PORTD |= (1 << 5)

//继电器关闭宏定义
#define ALARM_OFF PORTD &= ~(1 << 4)
#define ACCON_OFF PORTD &= ~(1 << 5)

//IO口模拟uart相关引脚定义
#define USART2_TX_PIN_OUTPUT DDRC |= (1 << 7)  //配置IO口模拟Usart	TX脚为输出
#define USART2_RX_PIN_INPUT DDRC &= ~(1 << 6)  //RX脚为输入
#define USART2_RX_PIN_PULLUP PORTC |= (1 << 6) //RX脚上拉

#define USART2_TX_PIN_SET PORTC |= (1 << 7)
#define USART2_TX_PIN_RESET PORTC &= ~(1 << 7)
#define USART2_RX_PIN_VALUE (PINC >> 6) & 0x01

void gpio_init();

#endif /* GPIO_H_ */