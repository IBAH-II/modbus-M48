#ifndef __UART_H
#define __UART_H

#if defined(__IAR_SYSTEMS_ASM__)

#define CLK_Uart (8000000)
#define UartSpeed (19200)
#define FIFORX (32)
#define FIFOTX (64)

#else

#include "main.h"
void UartInit(void);
unsigned char GetUart(void);
unsigned char InkeyUart(void);
unsigned short Inkey16Uart(void);
void PutUart(unsigned char a);

void putstrf(unsigned char __flash* a);
void puthex8(unsigned char a);
void puthex16(unsigned short a);
void puthex32(unsigned long a);

#define putstr(__A) \
{ \
  static __flash unsigned char __stroka[]=__A;\
  putstrf(__stroka);\
}

#endif

#endif



