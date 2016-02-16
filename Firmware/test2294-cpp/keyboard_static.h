
#ifndef _KEYBOARD_STATIC_H_
#define _KEYBOARD_STATIC_H_

#define KEYDIR IO0DIR

#include "ringbuffer.h"

typedef unsigned int            uint32;
typedef volatile unsigned long  vuint32;

#define	REG_BG3X_SUB		(*(vu32*)0x4001038)


struct KeyEvent
{
  int Make : 1;
  int Scancode : 8;
};

class KeyboardStatic
{

private:
  int _keys_old;
  int _keys_current;

  int _count;
  int _first;
  long _mask;
  vuint32 * _port;
  
  Ringbuffer<KeyEvent,30> _keybuffer;

public:
  KeyboardStatic(vuint32 * port, int first, int count);

public:
 void read();
 void bufferClear();
 int bufferLength();
 KeyEvent bufferPop();

};



#endif

