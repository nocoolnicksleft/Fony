
#ifndef _TICK_H_
#define _TICK_H_

extern "C" static void timertickISR(void) __attribute__ ((interrupt ("IRQ")));

class Tick
{

protected:
  volatile unsigned short int _timeout10msec;
  volatile unsigned short int _timeout400msec;

  volatile unsigned short int timer400msec;
  
  static Tick * pTick;

public:
  Tick();
  void init_tick_counter();
  int timeout10msec();
  int timeout400msec();

unsigned long ticks_per_second(void)
{
  return 1000000;
}

unsigned long ticks(void)
{
  return T0TC;
}

void delay_ticks(unsigned long t)
{
  unsigned long st = ticks();
  while ((ticks() - st) < t);
}

void delay_ms(unsigned long ms)
{
  delay_ticks((ticks_per_second() / 1000) * ms);
}

void delay_us(unsigned long us)
{
  delay_ticks((ticks_per_second() / 1000000) * us);
}

friend void timertickISR();

};

#endif

