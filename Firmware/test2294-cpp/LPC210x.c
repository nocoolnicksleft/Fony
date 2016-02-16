// CrossWorks Tasking Library.
//
// Timer support for the Philips LPC2xxx range
//
// Copyright (c) 2005 Rowley Associates Limited.
//
// This file may be distributed under the terms of the License Agreement
// provided with this software.
//
// THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

#include <ctl_api.h>
#include <targets/LPC2000.h>
#include <targets/liblpc2000.h>

#define TIMER0_INT 4

static CTL_ISR_FN_t timerFn;

static void
timerISR(void)
{
  timerFn();
  /* Clear the timer 0 interrupt */
  T0IR = 0xFF;
}

void
ctl_start_timer(CTL_ISR_FN_t t)
{
  timerFn = t;
  T0TCR = 0; /* Reset timer 0 */
  T0PR = 0; /* Set the timer 0 prescale counter */
  T0MR0 = (liblpc2000_get_pclk(liblpc2000_get_cclk(OSCILLATOR_CLOCK_FREQUENCY)) / 1000) - 1; /* Set timer 0 match register to generate a 1KHz interrupt */
  T0MCR = 3; /* Generate interrupt and reset counter on match */
  T0TCR = 1;   /* Start timer 0 */  
  ctl_set_isr(TIMER0_INT, 0, CTL_ISR_TRIGGER_FIXED, timerISR, 0);
  ctl_unmask_isr(TIMER0_INT);
}

unsigned long
ctl_get_ticks_per_second(void)
{
  return 1000;
}
