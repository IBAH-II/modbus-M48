#ifndef __WAIT_H
#define __WAIT_H

#include "main.h"

#define Use_uS_Timer (0) //использовать микросекундный таймер
#define Kvant_uS_Timer (50) //кавант времени таймера в микросекундах

//объект таймер
#pragma pack(push,1)
typedef struct
  {
  unsigned long t0;//опорное время
  unsigned long pt;//порог срабатывания 
  }Timer_t;
#pragma pack(pop) 

//объявление-инициализация глобальных переменных
#ifdef _WAIT_C
//переменная системного таймера
volatile unsigned long TimingDelay;
#else
extern volatile const unsigned long TimingDelay;
#endif

///////////////////////////////////////////////////////////////////////////////
//настройка SysTick
static inline void InitWait(void)
  {
#if Use_uS_Timer==0    
  SysTick_Config(SystemCoreClock/1000);
#else  
#if Kvant_uS_Timer>200  
  SysTick_Config((SystemCoreClock)/1000000*Kvant_uS_Timer);//при квантах >200uS
#else  
  SysTick_Config((SystemCoreClock*Kvant_uS_Timer)/1000000);
#endif  
#endif  
  NVIC_SetPriority(SysTick_IRQn, 0);
  }

//функция задержки 
static inline void Delay(unsigned long nTime)
  {
  unsigned long tmp=TimingDelay;
  while((TimingDelay-tmp)<nTime);
  }

//икремент системного таймера 
void IncSysTimer(unsigned long a);
  
///////////////////////////////////////////////////////////////////////////////
//методы объекта таймер

//старт таймера
//аргумент порог таймера
static inline void StartTimer(Timer_t* t, unsigned long pt)
  {
  t->t0=TimingDelay;
  t->pt=pt;
  }

//Главный метод таймера без перезапуска
//узнать дотикал-ли таймер
//результат
//0 - не дотикал
//!0 - дотикал
static inline unsigned char Timer(Timer_t* t)
  {
  return ((TimingDelay-(t->t0))>=(t->pt));
  }

//Главный метод таймера с перезапуском
//результат
//0 - не дотикал
//!0 - дотикал
static inline unsigned char TimerReStart(Timer_t* t)
  {
  if((TimingDelay-(t->t0))>=(t->pt))
    {
    t->t0=TimingDelay;
    return !0;
    }
  return 0;
  }

//Главный метод таймера с перезапуском и перезагрузкой
//результат
//0 - не дотикал
//!0 - дотикал
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

//Получить значение таймера
static inline unsigned long GetTimer(Timer_t* t)
  {
  return (TimingDelay-(t->t0));
  }

//Получить значение порога таймера
static inline unsigned long GetPorogTimer(Timer_t* t)
  {
  return t->pt;
  }


#endif //end  ifndef __WAIT_H
