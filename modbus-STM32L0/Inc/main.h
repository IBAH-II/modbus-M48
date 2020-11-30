#ifndef __MAIN_H
#define __MAIN_H
/*Includes ------------------------------------------------------------------*/
#include "stm32l0xx_ll_dma.h"
#include "stm32l0xx_ll_lpuart.h"
#include "stm32l0xx_ll_rcc.h"
#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_system.h"
#include "stm32l0xx_ll_cortex.h"
#include "stm32l0xx_ll_utils.h"
#include "stm32l0xx_ll_pwr.h"
#include "stm32l0xx.h"
#include "stm32l0xx_ll_gpio.h"
#include "stm32l0xx_ll_tim.h"

/* USER CODE Includes */
#include "SysConfig.h"
#include "wait.h"
#include "uart0.h"
#include "modbus.h" 
#include "ioport.h" 

///////////////////////////////////////////////////////////////////////////////

//описание ввода-вывода
//Выходы
///////////////////////////////////////////////////////////////////////////
//Входы
  
#ifdef _MAIN_C
unsigned char PoluSeconds;
unsigned char Seconds;
unsigned char Minutes;
unsigned char Hours;

unsigned short tmax;
unsigned short tmin=0xFFFF;
unsigned short tcurent;
unsigned short tavg;
#else 
extern unsigned char PoluSeconds;
extern unsigned char Seconds;
extern unsigned char Minutes;
extern unsigned char Hours;
extern unsigned short tmax;
extern unsigned short tmin;
extern unsigned short tcurent;
extern unsigned short tavg;
#endif 
///////////////////////////////////////////////////////////////////////////////

/* Private define ------------------------------------------------------------*/
#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1 */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

#endif /* __MAIN_H */

