#define __MODBUS2PRG_C
#include "modbus.h"

//Заполнение регистров Модбас
//перенос данных из программных переменных в регистры МодБас
void Prg2ModBusOutBit(void)
  {//заполнение регистров дискретных выходов
  ModBusOutBit[0].bit0=PORTC_Bit1;
  ModBusOutBit[0].bit1=PORTC_Bit2;
  ModBusOutBit[0].bit2=PORTC_Bit3;
  ModBusOutBit[0].bit3=PORTC_Bit4;
  return;
  }

void Prg2ModBusInBit(void)
  {//заполнение регистров дискретных входов
  ModBusInBit[0].bit0=PORTC_Bit1;
  ModBusInBit[0].bit1=PORTC_Bit2;
  ModBusInBit[0].bit2=PORTC_Bit3;
  ModBusInBit[0].bit3=PORTC_Bit4;
  ModBusInBit[0].bit4=PoluSeconds;
  return;
  }

void Prg2ModBusOutReg(void)
  {//заполнение регистров 4Х регистры для чтения/записи
  ModBusOutReg[0]=Seconds;
  ModBusOutReg[1]=Minutes;
  ModBusOutReg[2]=Hours; 
  return;
  }

void Prg2ModBusInReg(void)
  {//заполнение регистов 3Х регистры для чтения
  ModBusInReg[0]=Seconds;
  ModBusInReg[1]=Minutes;
  ModBusInReg[2]=Hours;  
  return;
  }

//Считывание регистров Модбас
//Перенос данных из регистров МодБас в программные переменные 
void ModBus2PrgOutBit(void)
  {//чтение регистров дискретных выходов
  PORTC_Bit1=ModBusOutBit[0].bit0;
  PORTC_Bit2=ModBusOutBit[0].bit1;
  PORTC_Bit3=ModBusOutBit[0].bit2;
  PORTC_Bit4=ModBusOutBit[0].bit3;
  return;
  }

void ModBus2PrgOutReg(void)
  {//чтение регистров 4Х регистры для чтения/записи
  Seconds=ModBusOutReg[0];
  Minutes=ModBusOutReg[1];
  Hours=ModBusOutReg[2]; 
  return;
  }


