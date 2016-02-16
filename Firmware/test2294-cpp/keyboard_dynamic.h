
#ifndef _KEYBOARD_DYNAMIC_H_
#define _KEYBOARD_DYNAMIC_H_

#include <libarm.h>
#include "common.h"
#include "ringbuffer.h"

typedef unsigned int            uint32;
typedef volatile unsigned long  vuint32;

struct KeyEvent
{
  unsigned short int Make;
  unsigned short int Scancode;
};

class KeyboardDynamic
{

private:
  unsigned short int _keys_old[5];
  unsigned short int _keys_current[5];

  // Number of pins for row input
  unsigned short int _count;

  // First pin of row input
  unsigned short int _first;

  // Mask of row input pins
  long _mask;

  // Port address for reading row input
  vuint32 * _port;

  // Number of pins for column output
  int _colcount;
  
  // First pin of column output
  int _colfirst;

  // Mask of column output pins
  long _colmask;

  // Port address for reading column output 
  vuint32 * _colread;
  
  // Port address for setting column output 
  vuint32 * _colclr;
  
  // Port address for clearing column output 
  vuint32 * _colset;

  // Buffer for recorded key status changes
  Ringbuffer<KeyEvent,30> _keybuffer;

public:
  KeyboardDynamic(vuint32 * port, int first, int count, vuint32 * colread, vuint32 * colclr, vuint32 * colset, int colfirst, int colcount);

public:
 
 // Read current key state (call in de-bouncing interval, about 10msec)
 void read();

 // Read current state, clear buffer
 void bufferClear();

 // Current buffer length (
 int bufferLength();

 // Pop latest KeyEvent off buffer
 KeyEvent bufferPop();

 void delay_ms(int n);

};



#endif

