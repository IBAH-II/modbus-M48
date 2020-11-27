#define __UART_ASM
#include <iom48.h>
#include "uart.h"
MODULE __uartrxtx

RSEG NEAR_Z
rxfifo: //Буфер FIFO
  DS FIFORX
rxHead://голова, пишем на голову
  DS 1 
rxTail://хвост, читаем с хвоста
  DS 1 
txfifo://Буфер FIFO
  DS FIFOTX
txHead://голова, пишем на голову
  DS 1 
txTail://хвост, читаем с хвоста и в UART
  DS 1 

COMMON INTVEC
ORG USART_RX_vect
  rjmp uart_rx
ORG USART_TX_vect
  rjmp uart_tx  

RSEG CODE
//void UartInit(void);
PUBLIC UartInit
UartInit:
  sbi PORTD,0
  cli
  push r16
  //обнуление указателей
  clr r16
  sts rxHead,r16
  sts rxTail,r16
  sts txHead,r16
  sts txTail,r16
  //Скорость передачи
  ldi r16,LOW((CLK_Uart/8+UartSpeed/2)/UartSpeed-1)
  sts UBRR0L,r16
  ldi r16,HIGH((CLK_Uart/8+UartSpeed/2)/UartSpeed-1)
  sts UBRR0H,r16
  //Enable receiver and transmitter, разрешение прерываний
  ldi r16,(1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(1<<TXCIE0)
  sts UCSR0B,r16
  //Set frame format: 8data, 1stop bit, Parity No
  ldi r16, (0<<UMSEL00)|(0<<UPM00)|(0<<USBS0)|(3<<UCSZ00)
  sts UCSR0C,r16
  //сброс флагов прерываний UART
  lds r16,UCSR0A
  ori r16,(1<<TXC0)|(1<<U2X0) 
  sts UCSR0A,r16
  lds r16,UDR0
  pop r16
  reti

//Обработчик прерывания по приему
uart_rx:
  push r16
  in r16,SREG
  push r16
  push XL
  push XH
//UART->FIFO  
  lds r16,rxHead
  ldi XL,LOW(rxfifo)
  ldi XH,HIGH(rxfifo)
  add XL,r16
  adc XH,r16
  sub XH,r16
  inc r16
  andi r16,(FIFORX-1)
  sts rxHead,r16
  lds r16,UDR0
  st X,r16
  pop XH
  pop XL    
  pop r16
  out SREG,r16
  pop r16
  reti

//unsigned short Inkey16Uart(void);
//проверка буфера, нет данных возвращает 0
PUBLIC Inkey16Uart
Inkey16Uart:
  lds R17,rxHead
  lds r16,rxTail
  cp r16,r17
  breq Inkey16Uart1
  //читаем данные из FIFO  
  push XL
  push XH
  ldi XL,LOW(rxfifo)
  ldi XH,HIGH(rxfifo)
  add XL,r16
  adc XH,r16
  sub XH,r16  
  inc r16
  andi r16,(FIFORX-1)
  sts rxTail,r16  
  ld r16,X
  pop XH
  pop XL
  ldi r17,1
  ret
Inkey16Uart1:
  clr r16
  clr r17
  ret

//обработчик прерывания по передаче
uart_tx:
  push r16
  in r16,SREG
  push r16
  push r17
  //проверяем наличие данных в буфере
  lds r17,txHead
  lds r16,txTail
  cp r16,r17
  brne uart_tx2  
  rjmp uart_tx_end
uart_tx2://если данные есть - передаем  
  push XL
  push XH
  ldi XL,LOW(txfifo)
  ldi XH,HIGH(txfifo)
  add XL,r16
  adc XH,r16
  sub XH,r16  
  inc r16
  andi r16,(FIFOTX-1)
  sts txTail,r16  
  ld r16,X
  sts UDR0,r16
  pop XH
  pop XL
uart_tx_end:
  pop r17
  pop r16
  out SREG,r16
  pop r16
  reti
  
//void PutUart(char a);
PUBLIC PutUart
PutUart:
  push XL
  push XH
//проверяем наличие данных в буфере
  lds XH,txHead
  lds XL,txTail
  cp XH,XL
  brne PutUart1
//проверякм регистр передачи
  lds XL,UCSR0A
  sbrs XL,UDRE0
  rjmp PutUart1
  sts UDR0,r16
  pop XH
  pop XL
  ret
PutUart1://положить в txfifo[]  
  push r16
  mov r16,XH
  ldi XL,LOW(txfifo)
  ldi XH,HIGH(txfifo)
  add XL,r16
  adc XH,r16
  sub XH,r16
  inc r16
  andi r16,(FIFOTX-1)
  sts txHead,r16
  pop r16
  st X,r16
  pop XH
  pop XL    
  ret
ENDMOD

MODULE __uartget
EXTERN rxHead
EXTERN rxTail
EXTERN rxfifo
//unsigned char GetUart(void);
PUBLIC GetUart
GetUart:
//проверяем наличие данных в буфере  
  push XL
GetUart1:  
  lds XL,rxHead
  lds r16,rxTail
  cp r16,XL
  breq GetUart1
//читаем данные из FIFO  
  push XH
  ldi XL,LOW(rxfifo)
  ldi XH,HIGH(rxfifo)
  add XL,r16
  adc XH,r16
  sub XH,r16  
  inc r16
  andi r16,(FIFORX-1)
  sts rxTail,r16  
  ld r16,X
  pop XH
  pop XL
  ret

ENDMOD

MODULE __uartinkey
EXTERN rxHead
EXTERN rxTail
EXTERN GetUart
//unsigned short InkeyUart(void);
//проверка буфера, нет данных возвращает 0
PUBLIC InkeyUart
InkeyUart:
  push r17
  lds r16,rxHead
  lds R17,rxTail
  sub r16,r17
  breq InkeyUart1
  rcall GetUart
InkeyUart1:
  pop r17
  ret
ENDMOD

MODULE __uartputstrf
EXTERN PutUart
//void putstrf(unsigned char __flash* a);
PUBLIC putstrf
putstrf: 
  push ZL
  push ZH
  mov ZL,r16
  mov ZH,r17
putstrf1:
  lpm r16,Z+
  tst r16
  breq putstrfend
  rcall PutUart
  rjmp putstrf1 
putstrfend:
  pop ZH 
  pop ZL 
  ret
ENDMOD

MODULE __uartputhex8
EXTERN PutUart
//void puthex8(unsigned long a);
PUBLIC	puthex8
puthex8:
  push r17
  push ZL
  push ZH
  mov r17,r16
  swap r16
  andi r16,0x0f
  andi r17,0x0f
puthex00:        
  ldi ZL,LOW(dt)
  ldi ZH,HIGH(dt)
puthex0:        
  add ZL,r16
  adc ZH,r16
  sub ZH,r16
  lpm r16,Z
  rcall PutUart
  cpi r17,128
  breq puthexend
  mov r16,r17
  ldi r17,128
  rjmp puthex00
puthexend:        
  pop ZH
  pop ZL
  pop r17 
  ret
dt:
  DB "0123456789ABCDEF" 
ENDMOD

MODULE __uartputhex16
EXTERN puthex8
//void puthex16(unsigned long a);
PUBLIC	puthex16
puthex16:
  push r16
  mov r16,r17
  rcall puthex8
  pop r16
  rcall puthex8
  ret
ENDMOD

MODULE __uartputhex32
EXTERN puthex8
//void puthex32(unsigned long a);
PUBLIC	puthex32
puthex32:
  push r16
  mov r16,r19
  rcall puthex8
  mov r16,r18
  rcall puthex8
  mov r16,r17
  rcall puthex8
  pop r16
  rcall puthex8
  ret
ENDMOD

END

