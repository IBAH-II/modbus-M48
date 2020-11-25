#define __MODBUS2PRG_C
#include "modbus.h"

//���������� ��������� ������
//������� ������ �� ����������� ���������� � �������� ������
void Prg2ModBusOutBit(void)
  {//���������� ��������� ���������� �������
  ModBusOutBit[0].byte<<=1;
  if(ModBusOutBit[0].byte==0)ModBusOutBit[0].byte=1;
  ModBusOutBit[1].byte<<=1;
  if(ModBusOutBit[1].byte==0)ModBusOutBit[1].byte=1;
  return;
  }

void Prg2ModBusInBit(void)
  {//���������� ��������� ���������� ������
  ModBusInBit[0].byte++;
  ModBusInBit[1].bit0=PORTC_Bit1;
  ModBusInBit[1].bit1=PORTC_Bit2;
  ModBusInBit[1].bit2=PORTC_Bit3;
  ModBusInBit[1].bit3=PORTC_Bit4;
  ModBusInBit[1].bit4=PORTC_Bit5;
  
  return;
  }

void Prg2ModBusOutReg(void)
  {//���������� ��������� 4� �������� ��� ������/������
  ModBusOutReg[0]++;
  ModBusOutReg[1]+=3;
  ModBusOutReg[2]+=5;  
  ModBusOutReg[3]+=7;
  ModBusOutReg[4]+=11;
  ModBusOutReg[5]+=13;  
  ModBusOutReg[6]+=17;
  ModBusOutReg[7]+=19;
  ModBusOutReg[8]+=23;  
  ModBusOutReg[9]+=29;
  ModBusOutReg[10]+=31;
  ModBusOutReg[11]+=37;  
  ModBusOutReg[12]+=39;
  ModBusOutReg[13]+=41;  
  ModBusOutReg[14]+=43;
  ModBusOutReg[15]++;
  return;
  }

void Prg2ModBusInReg(void)
  {//���������� �������� 3� �������� ��� ������
  ModBusInReg[0]++;
  ModBusInReg[1]+=3;
  ModBusInReg[2]+=5;  
  ModBusInReg[3]+=7;
  return;
  }

//���������� ��������� ������
//������� ������ �� ��������� ������ � ����������� ���������� 
void ModBus2PrgOutBit(void)
  {//������ ��������� ���������� �������
 
  return;
  }

void ModBus2PrgOutReg(void)
  {//������ ��������� 4� �������� ��� ������/������
  return;
  }


