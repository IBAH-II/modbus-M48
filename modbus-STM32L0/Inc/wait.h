#ifndef __WAIT_H
#define __WAIT_H

#include "main.h"

#define Use_uS_Timer (0) //������������ �������������� ������
#define Kvant_uS_Timer (50) //������ ������� ������� � �������������

//������ ������
#pragma pack(push,1)
typedef struct
  {
  unsigned long t0;//������� �����
  unsigned long pt;//����� ������������ 
  }Timer_t;
#pragma pack(pop) 

//����������-������������� ���������� ����������
#ifdef _WAIT_C
//���������� ���������� �������
volatile unsigned long TimingDelay;
#else
extern volatile const unsigned long TimingDelay;
#endif

///////////////////////////////////////////////////////////////////////////////
//��������� SysTick
static inline void InitWait(void)
  {
#if Use_uS_Timer==0    
  SysTick_Config(SystemCoreClock/1000);
#else  
#if Kvant_uS_Timer>200  
  SysTick_Config((SystemCoreClock)/1000000*Kvant_uS_Timer);//��� ������� >200uS
#else  
  SysTick_Config((SystemCoreClock*Kvant_uS_Timer)/1000000);
#endif  
#endif  
  NVIC_SetPriority(SysTick_IRQn, 0);
  }

//������� �������� 
static inline void Delay(unsigned long nTime)
  {
  unsigned long tmp=TimingDelay;
  while((TimingDelay-tmp)<nTime);
  }

//�������� ���������� ������� 
void IncSysTimer(unsigned long a);
  
///////////////////////////////////////////////////////////////////////////////
//������ ������� ������

//����� �������
//�������� ����� �������
static inline void StartTimer(Timer_t* t, unsigned long pt)
  {
  t->t0=TimingDelay;
  t->pt=pt;
  }

//������� ����� ������� ��� �����������
//������ �������-�� ������
//���������
//0 - �� �������
//!0 - �������
static inline unsigned char Timer(Timer_t* t)
  {
  return ((TimingDelay-(t->t0))>=(t->pt));
  }

//������� ����� ������� � ������������
//���������
//0 - �� �������
//!0 - �������
static inline unsigned char TimerReStart(Timer_t* t)
  {
  if((TimingDelay-(t->t0))>=(t->pt))
    {
    t->t0=TimingDelay;
    return !0;
    }
  return 0;
  }

//������� ����� ������� � ������������ � �������������
//���������
//0 - �� �������
//!0 - �������
static inline unsigned char TimerReStartNew(Timer_t* t, unsigned long pt)
  {
  if((TimingDelay-(t->t0))>=(t->pt))
    {
    t->pt=pt;
    t->t0=TimingDelay;
    return !0;
    }
  return 0;
  }

//�������� �������� �������
static inline unsigned long GetTimer(Timer_t* t)
  {
  return (TimingDelay-(t->t0));
  }

//�������� �������� ������ �������
static inline unsigned long GetPorogTimer(Timer_t* t)
  {
  return t->pt;
  }


#endif //end  ifndef __WAIT_H
