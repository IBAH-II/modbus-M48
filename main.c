#include <iom48.h>
#define __MAIN_C
#include "main.h"

Timer_t t1;
Timer_t t2;

void main(void)
  {
  DDRC_Bit4=1;
  DDRC_Bit3=1;
  DDRC_Bit2=1;
  DDRC_Bit1=1;
  DDRC_Bit0=1;
  PORTC_Bit3++;
  PORTC_Bit4++;
  StartTimer(&t1,500);
  StartTimer(&t2,501);
  InitSysClock();
  UartInit();
  ModBusIni();
  
  while(!0)
    {
    //ModBusASCII();
    ModBusRTU();
    if(TimerReStart(&t1))
      {
      PORTC_Bit1++;
      PORTC_Bit3++;
      }
    if(TimerReStart(&t2))
      {
      PORTC_Bit2++;
      PORTC_Bit4++;
      }
    }
  }





