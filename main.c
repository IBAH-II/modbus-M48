#define __MAIN_C
#include "main.h"

Timer_t t1;

void main(void)
  {
  DDRC_Bit4=1;
  DDRC_Bit3=1;
  DDRC_Bit2=1;
  DDRC_Bit1=1;
  DDRC_Bit0=1;
    
  InitSysClock();
  UartInit();
  ModBusIni();
  StartTimer(&t1,500);
  while(!0)
    {
    //ModBusASCII();
    ModBusRTU();
    if(TimerReStart(&t1))
      {
      PoluSeconds++;
      if(PoluSeconds==2)
        {
        PoluSeconds=0;
        Seconds++;
        if(Seconds==60)
          { 
          Seconds=0; 
          Minutes++;
          if(Minutes==60)
            {
            Minutes=0;
            Hours++;
            if(Hours==24)Hours=0;
            }
          }
        }
      }
    }
  }





