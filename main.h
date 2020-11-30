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

#endif


