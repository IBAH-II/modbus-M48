#ifndef __MAIN_H
#define __MAIN_H

#include <ina90.h>
#include <iom48.h>
#include "systimer.h"
#include "uart.h"
#include "modbus.h"

#ifdef __MAIN_C
Timer_t t1;
unsigned char PoluSeconds;
unsigned char Seconds;
unsigned char Minutes;
unsigned char Hours;
#else
extern unsigned char PoluSeconds;
extern unsigned char Seconds;
extern unsigned char Minutes;
extern unsigned char Hours;
#endif

#endif


