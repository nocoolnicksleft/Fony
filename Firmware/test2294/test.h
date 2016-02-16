// Copyright (c) 2004 Rowley Associates Limited.
//
// This file may be distributed under the terms of the License Agreement
// provided with this software.
//
// THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

#include <ctl_api.h>

extern unsigned long idleCount;

/* Initialise target board */
void BoardInit(void);

/* Setup a timer ISR */
void SetTimerISR(CTL_ISR_FN_t isr);

/* Return the number of timer ticks per second */
unsigned long TicksPerSecond(void);

/* Return the processor's clock frequency */
unsigned long ClockFrequency(void);

/* Setup a button press ISR */
void SetButtonPressedISR(CTL_ISR_FN_t isr);

/* Set target board LEDs */
void SetLeds(unsigned v);
 
/* Carry out any idle processing. */
void Idle(void);

extern void Task_2(void);
