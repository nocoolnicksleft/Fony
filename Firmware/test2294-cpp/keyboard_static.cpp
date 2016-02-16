
#include "keyboard_static.h"
#include <stdio.h>
#include <string.h>

#include <targets/lpc2294.h>

KeyboardStatic::KeyboardStatic(vuint32 * port, int first, int count)
{
  _port = port;
  _count = count;
  _first = first;

  _mask = 0;
  for (int i=0;i<_count;i++)
  {
    _mask |= (1 << i);
  }
  _mask <<= _first;
  
  bufferClear();
}

int KeyboardStatic::bufferLength()
{
  return _keybuffer.length();
}

KeyEvent KeyboardStatic::bufferPop()
{
  return _keybuffer.pop();
}

void KeyboardStatic::bufferClear()
{
  _keybuffer.clear();
  _keys_current = (((*_port) & _mask) >> _first);
  _keys_old = _keys_current;
}


void KeyboardStatic::read()
{
  _keys_current = (((*_port) & _mask) >> _first);

  if (_keys_current != _keys_old)
  {
    //debug_printf("KEY %d\n",keys_current);
    int i;
    int deltakeys = (_keys_current ^ _keys_old);
    for (i=0;i<_count;i++)
    {
      if (deltakeys & (1 << i))
      {
        KeyEvent event;
        if (_keys_current & (1 << i)) event.Make = 0;
        else event.Make = 1;
        event.Scancode = i;
        _keybuffer.push(event);
      }
    }
    _keys_old = _keys_current;
  }
}


