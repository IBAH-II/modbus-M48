#define __MAIN_C
#include "main.h"

Timer_t t1;
unsigned long avg32;
#define alfa (16)
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
  
  //Timer1 init
  TCCR1A=0;
  TCCR1B=1;
  
  
  while(!0)
    {
    TCNT1=0;
    ModBusRTU();
    //ModBusASCII();
    
    tcurent=TCNT1;
    if(tcurent<tmin)tmin=tcurent;
    if(tcurent>tmax)tmax=tcurent;
    avg32=avg32-(avg32>>alfa)+tcurent;
    tavg=avg32>>alfa;
    
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





