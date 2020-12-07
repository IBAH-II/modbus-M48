#ifndef __SYSTIMER_H
#define __SYSTIMER_H

#ifdef __SYSTIMER_ASM

#define CLKSysTimer (8000000/64)

#else

#include "main.h"
//�������������
void InitSysClock(void);
//����� �� ������� � ������������
unsigned long Clock(void);
//��������
static void Delay(unsigned long a)
  {
  unsigned long tmp=Clock();
  while((Clock()-tmp)<a);
  }

//������ ������
typedef struct
  {
  unsigned long t0;//������� �����
  unsigned long pt;//����� ������������ 
  }Timer_t;
//������ ������� ������
//����� �������//�������� ����� �������
static void StartTimer(Timer_t* t, unsigned long pt)
  {
  t->t0=Clock();
  t->pt=pt;
  }
//������� ����� ������� ��� �����������//������ �������-�� ������
//���������//0 - �� �������//!0 - �������
static unsigned char Timer(Timer_t* t)
  {
  return ((Clock()-(t->t0))>=(t->pt));
  }
//������� ����� ������� � ������������
//���������//0 - �� �������//!0 - �������
static unsigned char TimerReStart(Timer_t* t)
  {
  if((Clock()-(t->t0))>=(t->pt))
    {
    t->t0=Clock();
    return !0;
    }
  return 0;
  }
//������� ����� ������� � ������������ � �������������
//���������//0 - �� �������//!0 - �������
static unsigned char TimerReStartNew(Timer_t* t, unsigned long pt)
  {
  if((Clock()-(t->t0))>=(t->pt))
    {
    t->pt=pt;
    t->t0=Clock();
    return !0;
    }
  return 0;
  }

//�������� �������� �������
static unsigned long GetTimer(Timer_t* t)
  {
  return (Clock()-(t->t0));
  }

#endif

#endif
