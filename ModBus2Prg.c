#define __MODBUS2PRG_C
#include "modbus.h"

//���������� ��������� ������
//������� ������ �� ����������� ���������� � �������� ������
void Prg2ModBusOutBit(void)
  {//���������� ��������� ���������� �������
  ModBusOutBit[0].bit0=PORTC_Bit1;
  ModBusOutBit[0].bit1=PORTC_Bit2;
  ModBusOutBit[0].bit2=PORTC_Bit3;
  ModBusOutBit[0].bit3=PORTC_Bit4;
  return;
  }

void Prg2ModBusInBit(void)
  {//���������� ��������� ���������� ������
  ModBusInBit[0].bit0=PORTC_Bit1;
  ModBusInBit[0].bit1=PORTC_Bit2;
  ModBusInBit[0].bit2=PORTC_Bit3;
  ModBusInBit[0].bit3=PORTC_Bit4;
  ModBusInBit[0].bit4=PoluSeconds;
  return;
  }

void Prg2ModBusOutReg(void)
  {//���������� ��������� 4� �������� ��� ������/������
  ModBusOutReg[0]=Seconds;
  ModBusOutReg[1]=Minutes;
  ModBusOutReg[2]=Hours; 
  return;
  }

void Prg2ModBusInReg(void)
  {//���������� �������� 3� �������� ��� ������
  ModBusInReg[0]=Seconds;
  ModBusInReg[1]=Minutes;
  ModBusInReg[2]=Hours;  
  return;
  }

//���������� ��������� ������
//������� ������ �� ��������� ������ � ����������� ���������� 
void ModBus2PrgOutBit(void)
  {//������ ��������� ���������� �������
  PORTC_Bit1=ModBusOutBit[0].bit0;
  PORTC_Bit2=ModBusOutBit[0].bit1;
  PORTC_Bit3=ModBusOutBit[0].bit2;
  PORTC_Bit4=ModBusOutBit[0].bit3;
  return;
  }

void ModBus2PrgOutReg(void)
  {//������ ��������� 4� �������� ��� ������/������
  Seconds=ModBusOutReg[0];
  Minutes=ModBusOutReg[1];
  Hours=ModBusOutReg[2]; 
  return;
  }


