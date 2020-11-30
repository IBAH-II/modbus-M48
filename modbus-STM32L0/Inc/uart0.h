#ifndef __UART0_H
#define __UART0_H

#include "main.h"

#ifdef _UART0_C //��������� ������ ��� ������ UART0_C
//������ ������ FIFO [����], ������ ���� ����� ������� ������ (16,32,64...)
#define FifoRxBuf (256)
#define FifoTxBuf (256)
#endif 

//������������� FIFO
//�������� �������� ������
void FifoInit0(unsigned long Speed);
//��������������� �����
void FifoDeInit0(void);

//Function: �������� ���� � ���� 
//Argument: ������������ ����   
void PutFifo0(unsigned char data);

//Function:  - ����� ����� �� ����  
//Result:   �������� ���� 
unsigned char GetFifo0(void);

//������� ��������� ������
void FifoClear0(void);

//Function: ����� ����� ��� ��������                                
//Result:   �������� ����, ���� ���� �� ������ �� ������� 
unsigned char InkeyFifo0(void);
//Function: ����� ����� ��� ��������                                
//Result:���� ���� �������� ������  
//������� ���� �� ����
//������� ���� �������� ����
//���� ������ �� �������, �� ���� 
unsigned short Inkey16Fifo0(void);

//Function: ����� ������ � Fifo �� FLASH                                
//Result: 
void PutStrFifoF0(unsigned char const*);
//������ ������ ������ �� FLASH
#define putstrfifo0(__A) \
{ \
  static const unsigned char __stroka[]=__A;\
  PutStrFifoF0(__stroka);\
}

//Function:  ����� ������ �� RAM ->FIFO                                
void PutStrFifoM0(unsigned char*);

//����� � ���������� ���� �� ������
void PutDec8sFifo0(signed char);
void PutDec16sFifo0(signed short);
void PutDec32sFifo0(signed long);
//����� � ���������� ���� �� ������ �� �������, N ���������� ������
void PutDecNsFifo0(char N,signed long a);

//����� � ���������� ���� ��� �����
void PutDec8uFifo0(unsigned char);
void PutDec16uFifo0(unsigned short);
void PutDec32uFifo0(unsigned long);
//����� � ���������� ���� ��� ����� �� �������, N ���������� ������
void PutDecNuFifo0(char N,unsigned long a);
//����� � ���������� ���� ��� ����� ��� �������� �����
void PutDecuFifo0(unsigned long a);
//����� � ���������� ���� �� ������ ��� �������� �����
void PutDecsFifo0(signed long a);

//����� � ���������������� ����
void PutHex8Fifo0(unsigned char);
void PutHex16Fifo0(unsigned short);
void PutHex32Fifo0(unsigned long);
void PutHex64Fifo0(unsigned long long);

//����� � �������� ����
void PutBin8Fifo0(unsigned char);
void PutBin16Fifo0(unsigned short);
void PutBin32Fifo0(unsigned long);
void PutBin64Fifo0(unsigned long long);

//����� � �������� ���� ������� ������
void PutBinRev8Fifo0(unsigned char a);
void PutBinRev16Fifo0(unsigned short a);
void PutBinRev32Fifo0(unsigned long a);

//����� Float �� �������
//���������
//unsigned char pered ����� ������ ����� �������
//unsigned char posle ����� ������ ����� �������
void PutFloatFifo0(unsigned char pered,unsigned char posle,unsigned char znak,float a);

//����� double �� �������
//���������
//unsigned char pered ����� ������ ����� �������
//unsigned char posle ����� ������ ����� �������
void PutDoubleFifo0(unsigned char pered,unsigned char posle,unsigned char znak,double a);

#endif 