
#include "common.h"

#include "tick.h"

Tick * Tick::pTick = 0;

Tick::Tick()
{
    _timeout10msec = 0;
    timer400msec = 40;
    _timeout400msec = 0;

    pTick = this;
}

void Tick::init_tick_counter()
{
  T0TCR = 2;
  T0TCR = 0;
  T0PR = liblpc2000_get_pclk(liblpc2000_get_cclk(OSCILLATOR_CLOCK_FREQUENCY)) / ticks_per_second();
  T0TCR = 1;
}

int Tick::timeout10msec()
{
  if (_timeout10msec)
  {
    _timeout10msec = 0;
    return 1;
  }
  return 0;
}

int Tick::timeout400msec()
{
  if (_timeout400msec)
  {
    _timeout400msec = 0;
    return 1;
  }
  return 0;
}


extern "C" static void timertickISR(void)
{
   Tick::pTick->_timeout10msec = 1;

   if (!(--Tick::pTick->timer400msec)) {
    Tick::pTick->timer400msec = 40;
    Tick::pTick->_timeout400msec = 1;
   }

// VICSoftIntClear = 0xFFFFFFFF;  /* Clear the timer 0 interrupt */
  T0IR = 0xFF;
  /* Update VIC priorities */
  VICVectAddr = 0;
}

