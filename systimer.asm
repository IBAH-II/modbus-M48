#include <iom48.h>
#include "systimer.h"

MODULE __systimer

COMMON INTVEC
ORG TIMER0_COMPA_vect
  rjmp tim0_comp

RSEG CODE
tim0_comp:
  in r10,SREG
  inc r11
  add r12,r11
  dec r11
  adc r13,r11
  adc r14,r11
  adc r15,r11
  out SREG,r10
  reti

PUBLIC InitSysClock
InitSysClock:
  cli
  clr r11
  clr r12
  clr r13
  clr r14
  clr r15
  push r16
  ldi r16,(0<<COM0A1)|(0<<COM0A0)|(0<<COM0B1)|(0<<COM0B0)|(1<<WGM01)|(0<<WGM00)
  out TCCR0A,r16
  ldi r16,(0<<FOC0A)|(0<<FOC0B)|(0<<WGM02)|(3<<CS00)
  out TCCR0B,r16
  ldi r16,(CLKSysTimer/1000)
  out OCR0A,r16
  ldi r16,(0<<OCIE0B)|(1<<OCIE0A)|(0<<TOIE0)
  sts TIMSK0,r16
  pop r16
  reti

PUBLIC Clock
Clock:
  cli
  movw r16,r12
  movw r18,r14 
  reti

ENDMOD
END
