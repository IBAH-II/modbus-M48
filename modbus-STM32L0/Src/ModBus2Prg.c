#define __MODBUS2PRG_C
#include "modbus.h"

Out_t PortOut[KolvoPortov]; 

//���������� ��������� ������
//������� ������ �� ����������� ���������� � �������� ������
void Prg2ModBusOutBit(void)
  {//���������� ��������� ���������� �������
  ModBusOutBit[0].bit0=PortOut[0].out0;
  ModBusOutBit[0].bit1=PortOut[0].out1;
  ModBusOutBit[0].bit2=PortOut[0].out2;
  ModBusOutBit[0].bit3=PortOut[0].out3;
  return;
  }

void Prg2ModBusInBit(void)
  {//���������� ��������� ���������� ������
  ModBusInBit[0].bit0=PortOut[0].out0;
  ModBusInBit[0].bit1=PortOut[0].out1;
  ModBusInBit[0].bit2=PortOut[0].out2;
  ModBusInBit[0].bit3=PortOut[0].out3;
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
  ModBusInReg[3]=tcurent;
  ModBusInReg[4]=tmax;
  ModBusInReg[5]=tmin;
  ModBusInReg[6]=tavg;
  return;
  }

//���������� ��������� ������
//������� ������ �� ��������� ������ � ����������� ���������� 
void ModBus2PrgOutBit(void)
  {//������ ��������� ���������� �������
  PortOut[0].out0=ModBusOutBit[0].bit0;
  PortOut[0].out1=ModBusOutBit[0].bit1;
  PortOut[0].out2=ModBusOutBit[0].bit2;
  PortOut[0].out3=ModBusOutBit[0].bit3;
  return;
  }

void ModBus2PrgOutReg(void)
  {//������ ��������� 4� �������� ��� ������/������
  Seconds=ModBusOutReg[0];
  Minutes=ModBusOutReg[1];
  Hours=ModBusOutReg[2]; 
  return;
  }


