#define __MODBUS2PRG_C
#include "modbus.h"

Out_t PortOut[KolvoPortov]; 

//Заполнение регистров Модбас
//перенос данных из программных переменных в регистры МодБас
void Prg2ModBusOutBit(void)
  {//заполнение регистров дискретных выходов
  ModBusOutBit[0].bit0=PortOut[0].out0;
  ModBusOutBit[0].bit1=PortOut[0].out1;
  ModBusOutBit[0].bit2=PortOut[0].out2;
  ModBusOutBit[0].bit3=PortOut[0].out3;
  return;
  }

void Prg2ModBusInBit(void)
  {//заполнение регистров дискретных входов
  ModBusInBit[0].bit0=PortOut[0].out0;
  ModBusInBit[0].bit1=PortOut[0].out1;
  ModBusInBit[0].bit2=PortOut[0].out2;
  ModBusInBit[0].bit3=PortOut[0].out3;
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
  ModBusInReg[3]=tcurent;
  ModBusInReg[4]=tmax;
  ModBusInReg[5]=tmin;
  ModBusInReg[6]=tavg;
  return;
  }

//Считывание регистров Модбас
//Перенос данных из регистров МодБас в программные переменные 
void ModBus2PrgOutBit(void)
  {//чтение регистров дискретных выходов
  PortOut[0].out0=ModBusOutBit[0].bit0;
  PortOut[0].out1=ModBusOutBit[0].bit1;
  PortOut[0].out2=ModBusOutBit[0].bit2;
  PortOut[0].out3=ModBusOutBit[0].bit3;
  return;
  }

void ModBus2PrgOutReg(void)
  {//чтение регистров 4Х регистры для чтения/записи
  Seconds=ModBusOutReg[0];
  Minutes=ModBusOutReg[1];
  Hours=ModBusOutReg[2]; 
  return;
  }


