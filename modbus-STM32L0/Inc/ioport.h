#ifndef __IOPORT_H
#define __IOPORT_H

#include "main.h"

//������������ �����
void PortInit(void);
//�������������� �����
void PortDeInit(void);
//������-������ �����
void PortRW(void);

#pragma pack(push,1)
//��������� ������ ����� �����
typedef union
  {
  unsigned char in8; //���� �������
  struct
    {
    unsigned char in0:1; //���� in0
    unsigned char in1:1; //���� in1
    unsigned char in2:1; //���� in2
    unsigned char in3:1; //���� in3
    unsigned char in4:1; //���� in4
    unsigned char in5:1; //���� in5
    unsigned char in6:1; //���� in6
    unsigned char in7:1; //���� in7
    };
  }In_t;
//��������� ������ ����� ������
typedef union
  {
  unsigned char out8; //���� �������
  struct
    {
    unsigned char out0:1; //���� out0
    unsigned char out1:1; //���� out1
    unsigned char out2:1; //���� out2
    unsigned char out3:1; //���� out3
    unsigned char out4:1; //���� out4
    unsigned char out5:1; //���� out5
    unsigned char out6:1; //���� out6
    unsigned char out7:1; //���� out7
    };
  }Out_t;

#pragma pack(pop)

//���������� ������ ������ �����-������
#define KolvoPortov (1)

#ifdef _IOPORT_C //������ ��� ������ _IOPORT_C

//�������� ������ ������
//0  - ���������� ������ � 0 ��� ����������
//!0 - ���������� ������ � ����������
#define Port0orZ (1) 

//����� SPI
//IN_LOAD
#define IN_LOAD_AHB   LL_IOP_GRP1_PERIPH_GPIOB
#define IN_LOAD_Port  GPIOB
#define IN_LOAD       LL_GPIO_PIN_14
#define Set0IN_LOAD() IN_LOAD_Port->BRR=IN_LOAD
#define Set1IN_LOAD() IN_LOAD_Port->BSRR=IN_LOAD
//OUT_LOAD
#define OUT_LOAD_AHB   LL_IOP_GRP1_PERIPH_GPIOB
#define OUT_LOAD_Port  GPIOB
#define OUT_LOAD       LL_GPIO_PIN_13
#define Set0OUT_LOAD() OUT_LOAD_Port->BRR=OUT_LOAD
#define Set1OUT_LOAD() OUT_LOAD_Port->BSRR=OUT_LOAD
//OUT_DATA
#define OUT_DATA_AHB   LL_IOP_GRP1_PERIPH_GPIOB
#define OUT_DATA_Port  GPIOB
#define OUT_DATA       LL_GPIO_PIN_12
#define Set0OUT_DATA() OUT_DATA_Port->BRR=OUT_DATA
#define Set1OUT_DATA() OUT_DATA_Port->BSRR=OUT_DATA
//CLK_IO
#define CLK_IO_AHB   LL_IOP_GRP1_PERIPH_GPIOB
#define CLK_IO_Port  GPIOB
#define CLK_IO       LL_GPIO_PIN_2
#define Set0CLK_IO() CLK_IO_Port->BRR=CLK_IO
#define Set1CLK_IO() CLK_IO_Port->BSRR=CLK_IO
//PWR
#define PWR_PORT_AHB   LL_IOP_GRP1_PERIPH_GPIOB
#define PWR_PORT_Port  GPIOB
#define PWR_PORT  LL_GPIO_PIN_8
#define Set0PWR_PORT() PWR_PORT_Port->BRR=PWR_PORT
#define Set1PWR_PORT() PWR_PORT_Port->BSRR=PWR_PORT


//
//#define Set0CLK_IO_NNN() {Set0CLK_IO();Set0CLK_IO();Set0CLK_IO();Set0CLK_IO();}
//#define Set0CLK_IO_NNN() {Set0CLK_IO();Set0CLK_IO();Set0CLK_IO();}
//#define Set0CLK_IO_NNN() {Set0CLK_IO();Set0CLK_IO();}
//#define Set0CLK_IO_NNN() {Set0CLK_IO();}
//
//#define Set1CLK_IO_NNN() {Set1CLK_IO();Set1CLK_IO();Set1CLK_IO();Set1CLK_IO();}
//#define Set1CLK_IO_NNN() {Set1CLK_IO();Set1CLK_IO();Set1CLK_IO();}
//#define Set1CLK_IO_NNN() {Set1CLK_IO();Set1CLK_IO();}
#define Set1CLK_IO_NNN() {Set1CLK_IO();}

//IN_DATA
#define IN_DATA_AHB LL_IOP_GRP1_PERIPH_GPIOB
#define IN_DATA_Port GPIOB
#define IN_DATA LL_GPIO_PIN_0
#define GetIN_DATA() ((IN_DATA_Port->IDR)&(IN_DATA))

Out_t PortOut[KolvoPortov];
In_t  PortIn[KolvoPortov];
#else
extern Out_t PortOut[KolvoPortov];
extern In_t  PortIn[KolvoPortov];

#endif //#ifdef _IOPORT_C //��������� ������ ��� ������ _IOPORT_C

#endif //end #ifndef __IOPORT_H