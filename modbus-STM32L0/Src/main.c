#define _MAIN_C

//Includes
#include "main.h"

unsigned long avg32;
#define alfa (16)

int main(void)
  {
  {// MCU Configuration
  //Reset of all peripherals, Initializes the Flash interface and the Systick.
  LL_Init();
  //Configure the system clock
  SystemClock_Config();
  }
  {//Initialize all configured peripherals
  InitWait();//Время - таймеры
  FifoInit0(19200);//Инициализация UART FIFO 
  }
  
  putstrfifo0("\n\r\n\rGo!\n\r\n\r");
  
  Timer_t t1;
  StartTimer(&t1,500);//таймер 1 секунда
  
  //TIM6 Init
  LL_TIM_InitTypeDef TIM_InitStruct;
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);
  TIM_InitStruct.Prescaler = 0;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 65533;
  LL_TIM_Init(TIM6, &TIM_InitStruct);
  LL_TIM_EnableCounter(TIM6);
  
  //////////////////////////////////////////////////////////////////////////////
  //главный цикл
  while(1)
    {
    TIM6->CNT=0;
    ModBusRTU();//обработка сообщений Модбас RTU
    //ModBusASCII();//обработка сообщений Модбас ASCII
    
    tcurent=TIM6->CNT;
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
