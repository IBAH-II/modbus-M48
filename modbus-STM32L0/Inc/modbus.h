///////////////////////////////////////////////////////////////////////////////
//             Асинхронная обработка сообщение ModBus v3                     //
//             Автор - Iван                                                  //
///////////////////////////////////////////////////////////////////////////////
#ifndef __MODBUS_H
#define __MODBUS_H

#include "main.h"

///////////////////////////////////////////////////////////////////////////////
//Настройки 
//Вычисление CRC
#define ModBusUseTableCRC (0) //Использовать вычисление СRС по таблице
//Обработка ошибок
#define ModBusUseErrMes (1) //Использовать сообщения о ошибках
//Данные, регистры ModBus 
#define ModBusUseGlobal (0) //Использовать глобальные входы/выходы, входные/выходные регистры
//Функции протокола ModBus
#define ModBusUseFunc1  (1) //Использовать функцию 1  - чтение статуса Coils (дискретных выходных битов)
#define ModBusUseFunc2  (1) //Использовать функцию 2  - чтение статуса дискретных входов
#define ModBusUseFunc3  (1) //Использовать функцию 3  - чтение значения выходных регистров
#define ModBusUseFunc4  (1) //Использовать функцию 4  - чтение значения входных регистров
#define ModBusUseFunc5  (1) //Использовать функцию 5  - запись выходного бита
#define ModBusUseFunc6  (1) //Использовать функцию 6  - запись выходного регистра
#define ModBusUseFunc15 (1) //Использовать функцию 15 - запись нескольких выходных битов
#define ModBusUseFunc16 (1) //Использовать функцию 16 - запись нескольких выходных регистров
#define ModBusUseFunc22 (1) //Использовать функцию 22 - запись выходного регистра по маске
//Адрес устройства
#define ModBusID (1) //Адрес на шине ModBus
#define ModBusID_FF (255) //Адрес на шине ModBus, на который отвечает всегда
//Таймауты
#define ModBusMaxPause (5)//Пауза между символами, для определения начала пакета [mS], 
#define ModBusMaxPauseResp (2) //Пауза между запросом Master и ответом Slave [mS]
//Длинна пакетов
#define ModBusMaxPaketRX (96)//Максимальный размер принимаемого пакета <255(RTU) <127(ASCII)
//Дискретные входы
#define ModBusMaxInBit (16) //Количество дискретных входов
#define ModBusMaxInBitTX (16) //Максимальное количество дискретных входов передаваемом пакете 
#define ModBusMaxInByte ((ModBusMaxInBit+7)/8) //Количество дискретных входов в байтах
//Дискретные выходы
#define ModBusMaxOutBit (16) //Количество дискретных выходов
#define ModBusMaxOutByte ((ModBusMaxOutBit+7)/8) //Количество дискретных выходов в байтах
#define ModBusMaxOutBitTX (16) //Максимальное количество дискретных выходов в передаваемом пакете  
#define ModBusMaxOutBitRX (8) //Максимальное количество дискретных выходов доступное для груповой установки (ModBusMaxOutBitRX/8+8)<(ModBusMaxPaketRX)
//Регистры доступные для чтения
#define ModBusMaxInReg (7) //Количество входных регистров (регистры только для чтения)
#define ModBusMaxInRegTX (8) //Максимальное количество входных регистров в передаваемом пакете 
//Регистры доступные для чтения-записи
#define ModBusMaxOutReg (4) //Количество выходных регистров
#define ModBusMaxOutRegTX (8)//Максимальное количество выходных регистров в передаваемом пакете 
#define ModBusMaxOutRegRX (8)//Максимальное количество выходных регистров  доступное для груповой установки (ModBusMaxOutRegRX+8)<(ModBusMaxPaketRX)
////////////////////////////////////////////////////////////////////////////////
//Опорные функции, связь с системой
//Системный таймер, инкрементируется каждую милисекунду
#define ModBusSysTimer TimingDelay
//Запись байта в поток последовательго порта - void ModBusPUT(unsigned char A)
#define ModBusPUT(A) PutFifo0(A) 
//Чтение байта из потока последовательного порта, - unsigned short ModBusGET(void)
//Если нет данных возвращает 0х0000, иначе возвращает 0х01ХХ
#define ModBusGET()  Inkey16Fifo0() 
////////////////////////////////////////////////////////////////////////////////

//Инициализация ModBus 
void ModBusIni(void);

//Функция обработки Сообщений ModBus RTU
//Работает совместно с системным таймером 
//Использует макросы ModBusPUT(A) ModBusGET()
void ModBusRTU(void);

//Функция обработки Сообщений ModBus ASCII
//Работает совместно с системным таймером 
//Использует макросы ModBusPUT(A) ModBusGET()
void ModBusASCII(void);

//Заполнение регистров ModBus
//перенос данных из программных переменных в регистры ModBus
void Prg2ModBusOutBit(void);
void Prg2ModBusInBit(void);
void Prg2ModBusOutReg(void);
void Prg2ModBusInReg(void);
//Считывание регистров ModBus
//перенос данных из регистров ModBus в программные переменные
void ModBus2PrgOutBit(void);
void ModBus2PrgOutReg(void);

#pragma pack(push,1)
//Тип данных для работы с дискретными входами/выходами
typedef union
  {
  unsigned char byte;
  struct
    {
    unsigned char bit0:1;
    unsigned char bit1:1;
    unsigned char bit2:1;
    unsigned char bit3:1;
    unsigned char bit4:1;
    unsigned char bit5:1;
    unsigned char bit6:1;
    unsigned char bit7:1;
    };
  }
  ModBusBit_t;
#pragma pack(pop)
  
#ifdef __MODBUS2PRG_C
#if ModBusMaxInBit!=0
ModBusBit_t ModBusInBit[ModBusMaxInByte]; //массив дискретных входов
#endif
#if ModBusMaxOutBit!=0
ModBusBit_t ModBusOutBit[ModBusMaxOutByte]; //массив дискретных выходов
#endif
#if ModBusMaxInReg!=0
unsigned short ModBusInReg[ModBusMaxInReg]; //массив входных регистров
#endif
#if ModBusMaxOutReg!=0
unsigned short ModBusOutReg[ModBusMaxOutReg]; //массив выходных регистров
#endif
#else 
#if ModBusUseGlobal!=0 || defined(__MODBUS_C)
#if ModBusMaxInBit!=0
extern ModBusBit_t ModBusInBit[ModBusMaxInByte]; //массив дискретных входов
#endif
#if ModBusMaxOutBit!=0
extern ModBusBit_t ModBusOutBit[ModBusMaxOutByte]; //массив дискретных выходов
#endif
#if ModBusMaxInReg!=0
extern unsigned short ModBusInReg[ModBusMaxInReg]; //массив входных регистров
#endif
#if ModBusMaxOutReg!=0
extern unsigned short ModBusOutReg[ModBusMaxOutReg]; //массив выходных регистров
#endif
#endif//#if ModBusUseGlobal!=0
#endif//#ifdef __MODBUS2PRG_C
#endif//#ifndef __MODBUS_H