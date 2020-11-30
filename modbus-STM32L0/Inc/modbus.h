///////////////////////////////////////////////////////////////////////////////
//             ����������� ��������� ��������� ModBus v3                     //
//             ����� - I���                                                  //
///////////////////////////////////////////////////////////////////////////////
#ifndef __MODBUS_H
#define __MODBUS_H

#include "main.h"

///////////////////////////////////////////////////////////////////////////////
//��������� 
//���������� CRC
#define ModBusUseTableCRC (0) //������������ ���������� �R� �� �������
//��������� ������
#define ModBusUseErrMes (1) //������������ ��������� � �������
//������, �������� ModBus 
#define ModBusUseGlobal (0) //������������ ���������� �����/������, �������/�������� ��������
//������� ��������� ModBus
#define ModBusUseFunc1  (1) //������������ ������� 1  - ������ ������� Coils (���������� �������� �����)
#define ModBusUseFunc2  (1) //������������ ������� 2  - ������ ������� ���������� ������
#define ModBusUseFunc3  (1) //������������ ������� 3  - ������ �������� �������� ���������
#define ModBusUseFunc4  (1) //������������ ������� 4  - ������ �������� ������� ���������
#define ModBusUseFunc5  (1) //������������ ������� 5  - ������ ��������� ����
#define ModBusUseFunc6  (1) //������������ ������� 6  - ������ ��������� ��������
#define ModBusUseFunc15 (1) //������������ ������� 15 - ������ ���������� �������� �����
#define ModBusUseFunc16 (1) //������������ ������� 16 - ������ ���������� �������� ���������
#define ModBusUseFunc22 (1) //������������ ������� 22 - ������ ��������� �������� �� �����
//����� ����������
#define ModBusID (1) //����� �� ���� ModBus
#define ModBusID_FF (255) //����� �� ���� ModBus, �� ������� �������� ������
//��������
#define ModBusMaxPause (5)//����� ����� ���������, ��� ����������� ������ ������ [mS], 
#define ModBusMaxPauseResp (2) //����� ����� �������� Master � ������� Slave [mS]
//������ �������
#define ModBusMaxPaketRX (96)//������������ ������ ������������ ������ <255(RTU) <127(ASCII)
//���������� �����
#define ModBusMaxInBit (16) //���������� ���������� ������
#define ModBusMaxInBitTX (16) //������������ ���������� ���������� ������ ������������ ������ 
#define ModBusMaxInByte ((ModBusMaxInBit+7)/8) //���������� ���������� ������ � ������
//���������� ������
#define ModBusMaxOutBit (16) //���������� ���������� �������
#define ModBusMaxOutByte ((ModBusMaxOutBit+7)/8) //���������� ���������� ������� � ������
#define ModBusMaxOutBitTX (16) //������������ ���������� ���������� ������� � ������������ ������  
#define ModBusMaxOutBitRX (8) //������������ ���������� ���������� ������� ��������� ��� �������� ��������� (ModBusMaxOutBitRX/8+8)<(ModBusMaxPaketRX)
//�������� ��������� ��� ������
#define ModBusMaxInReg (7) //���������� ������� ��������� (�������� ������ ��� ������)
#define ModBusMaxInRegTX (8) //������������ ���������� ������� ��������� � ������������ ������ 
//�������� ��������� ��� ������-������
#define ModBusMaxOutReg (4) //���������� �������� ���������
#define ModBusMaxOutRegTX (8)//������������ ���������� �������� ��������� � ������������ ������ 
#define ModBusMaxOutRegRX (8)//������������ ���������� �������� ���������  ��������� ��� �������� ��������� (ModBusMaxOutRegRX+8)<(ModBusMaxPaketRX)
////////////////////////////////////////////////////////////////////////////////
//������� �������, ����� � ��������
//��������� ������, ���������������� ������ �����������
#define ModBusSysTimer TimingDelay
//������ ����� � ����� ��������������� ����� - void ModBusPUT(unsigned char A)
#define ModBusPUT(A) PutFifo0(A) 
//������ ����� �� ������ ����������������� �����, - unsigned short ModBusGET(void)
//���� ��� ������ ���������� 0�0000, ����� ���������� 0�01��
#define ModBusGET()  Inkey16Fifo0() 
////////////////////////////////////////////////////////////////////////////////

//������������� ModBus 
void ModBusIni(void);

//������� ��������� ��������� ModBus RTU
//�������� ��������� � ��������� �������� 
//���������� ������� ModBusPUT(A) ModBusGET()
void ModBusRTU(void);

//������� ��������� ��������� ModBus ASCII
//�������� ��������� � ��������� �������� 
//���������� ������� ModBusPUT(A) ModBusGET()
void ModBusASCII(void);

//���������� ��������� ModBus
//������� ������ �� ����������� ���������� � �������� ModBus
void Prg2ModBusOutBit(void);
void Prg2ModBusInBit(void);
void Prg2ModBusOutReg(void);
void Prg2ModBusInReg(void);
//���������� ��������� ModBus
//������� ������ �� ��������� ModBus � ����������� ����������
void ModBus2PrgOutBit(void);
void ModBus2PrgOutReg(void);

#pragma pack(push,1)
//��� ������ ��� ������ � ����������� �������/��������
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
ModBusBit_t ModBusInBit[ModBusMaxInByte]; //������ ���������� ������
#endif
#if ModBusMaxOutBit!=0
ModBusBit_t ModBusOutBit[ModBusMaxOutByte]; //������ ���������� �������
#endif
#if ModBusMaxInReg!=0
unsigned short ModBusInReg[ModBusMaxInReg]; //������ ������� ���������
#endif
#if ModBusMaxOutReg!=0
unsigned short ModBusOutReg[ModBusMaxOutReg]; //������ �������� ���������
#endif
#else 
#if ModBusUseGlobal!=0 || defined(__MODBUS_C)
#if ModBusMaxInBit!=0
extern ModBusBit_t ModBusInBit[ModBusMaxInByte]; //������ ���������� ������
#endif
#if ModBusMaxOutBit!=0
extern ModBusBit_t ModBusOutBit[ModBusMaxOutByte]; //������ ���������� �������
#endif
#if ModBusMaxInReg!=0
extern unsigned short ModBusInReg[ModBusMaxInReg]; //������ ������� ���������
#endif
#if ModBusMaxOutReg!=0
extern unsigned short ModBusOutReg[ModBusMaxOutReg]; //������ �������� ���������
#endif
#endif//#if ModBusUseGlobal!=0
#endif//#ifdef __MODBUS2PRG_C
#endif//#ifndef __MODBUS_H