#ifndef __UART0_H
#define __UART0_H

#include "main.h"

#ifdef _UART0_C //константы только для модуля UART0_C
//размер буфера FIFO [байт], должно быть равно степени двойки (16,32,64...)
#define FifoRxBuf (256)
#define FifoTxBuf (256)
#endif 

//инициализация FIFO
//аргумент скорость обмена
void FifoInit0(unsigned long Speed);
//деинициализация порта
void FifoDeInit0(void);

//Function: положить байт в ФИФО 
//Argument: передаваемый байт   
void PutFifo0(unsigned char data);

//Function:  - прием байта из ФИФО  
//Result:   принятый байт 
unsigned char GetFifo0(void);

//очистка приемного буфера
void FifoClear0(void);

//Function: прием байта без ожидания                                
//Result:   принятый байт, если ноль то ничего не принято 
unsigned char InkeyFifo0(void);
//Function: прием байта без ожидания                                
//Result:если есть принятые данные  
//старший байт не ноль
//младший байт принятый байт
//если ничего не принято, то ноль 
unsigned short Inkey16Fifo0(void);

//Function: вывод строки в Fifo из FLASH                                
//Result: 
void PutStrFifoF0(unsigned char const*);
//макрос вывода строки из FLASH
#define putstrfifo0(__A) \
{ \
  static const unsigned char __stroka[]=__A;\
  PutStrFifoF0(__stroka);\
}

//Function:  вывод строки из RAM ->FIFO                                
void PutStrFifoM0(unsigned char*);

//вывод в десятичном виде со знаком
void PutDec8sFifo0(signed char);
void PutDec16sFifo0(signed short);
void PutDec32sFifo0(signed long);
//вывод в десятичном виде со знаком по формату, N количество знаков
void PutDecNsFifo0(char N,signed long a);

//вывод в десятичном виде без знака
void PutDec8uFifo0(unsigned char);
void PutDec16uFifo0(unsigned short);
void PutDec32uFifo0(unsigned long);
//вывод в десятичном виде без знака по формату, N количество знаков
void PutDecNuFifo0(char N,unsigned long a);
//вывод в десятичном виде без знака без значащий нулей
void PutDecuFifo0(unsigned long a);
//вывод в десятичном виде со знаком без значащий нулей
void PutDecsFifo0(signed long a);

//вывод в шеснадцатиричном виде
void PutHex8Fifo0(unsigned char);
void PutHex16Fifo0(unsigned short);
void PutHex32Fifo0(unsigned long);
void PutHex64Fifo0(unsigned long long);

//вывод в двоичном виде
void PutBin8Fifo0(unsigned char);
void PutBin16Fifo0(unsigned short);
void PutBin32Fifo0(unsigned long);
void PutBin64Fifo0(unsigned long long);

//вывод в двоичном виде младшим вперед
void PutBinRev8Fifo0(unsigned char a);
void PutBinRev16Fifo0(unsigned short a);
void PutBinRev32Fifo0(unsigned long a);

//вывод Float по формату
//аргументы
//unsigned char pered число знаков перед запятой
//unsigned char posle число знаков после запятой
void PutFloatFifo0(unsigned char pered,unsigned char posle,unsigned char znak,float a);

//вывод double по формату
//аргументы
//unsigned char pered число знаков перед запятой
//unsigned char posle число знаков после запятой
void PutDoubleFifo0(unsigned char pered,unsigned char posle,unsigned char znak,double a);

#endif 