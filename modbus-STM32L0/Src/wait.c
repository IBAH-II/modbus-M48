#define _WAIT_C

#include "main.h"
#include "wait.h"

//преравание системного таймера
void SysTick_Handler(void);
void SysTick_Handler(void)
  {
#if Use_uS_Timer==0  
  TimingDelay++;
#else
  TimingDelay+=Kvant_uS_Timer;
#endif  
  }

//икремент системного таймера 
void IncSysTimer(unsigned long a)
  {
  TimingDelay+=a;
  }


