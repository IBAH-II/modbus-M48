#ifndef __SYSTIMER_H
#define __SYSTIMER_H

#ifdef __SYSTIMER_ASM

#define CLKSysTimer (8000000/64)

#else

#include "main.h"
//Инициализация
void InitSysClock(void);
//время от запуска в милисекундах
unsigned long Clock(void);
//Задержка
static void Delay(unsigned long a)
  {
  unsigned long tmp=Clock();
  while((Clock()-tmp)<a);
  }

//объект таймер
typedef struct
  {
  unsigned long t0;//опорное время
  unsigned long pt;//порог срабатывания 
  }Timer_t;
//методы объекта таймер
//старт таймера//аргумент порог таймера
static void StartTimer(Timer_t* t, unsigned long pt)
  {
  t->t0=Clock();
  t->pt=pt;
  }
//Главный метод таймера без перезапуска//узнать дотикал-ли таймер
//результат//0 - не дотикал//!0 - дотикал
static unsigned char Timer(Timer_t* t)
  {
  return ((Clock()-(t->t0))>=(t->pt));
  }
//Главный метод таймера с перезапуском
//результат//0 - не дотикал//!0 - дотикал
static unsigned char TimerReStart(Timer_t* t)
  {
  if((Clock()-(t->t0))>=(t->pt))
    {
    t->t0=Clock();
    return !0;
    }
  return 0;
  }
//Главный метод таймера с перезапуском и перезагрузкой
//результат//0 - не дотикал//!0 - дотикал
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

//Получить значение таймера
static unsigned long GetTimer(Timer_t* t)
  {
  return (Clock()-(t->t0));
  }

#endif

#endif
