
#include "keyboard_dynamic.h"

KeyboardDynamic::KeyboardDynamic(vuint32 * port, int first, int count, vuint32 * colread, vuint32 * colclr, vuint32 * colset, int colfirst, int colcount)
{
  _port = port;
  _count = count;
  _first = first;

  _mask = 0;
  for (int i=0;i<_count;i++)
  {
    _mask |= (1 << i);
  }
  _mask <<= (_first);

  _colread = colread;
  _colclr = colclr;
  _colset = colset;
  _colcount = colcount;
  _colfirst = colfirst;

  _colmask = 0;
  for (int i=0;i<_colcount;i++)
  {
    _colmask |= (1 << i);
  }
  _colmask <<= (_colfirst);

  
  bufferClear();
}

int KeyboardDynamic::bufferLength()
{
  return _keybuffer.length();
}

KeyEvent KeyboardDynamic::bufferPop()
{
  return _keybuffer.pop();
}

void KeyboardDynamic::bufferClear()
{
  _keybuffer.clear();
  for (int currentcol = 0; currentcol < _colcount; currentcol++)
  {
    (*_colclr) = _colmask;
    (*_colset) = (1 << currentcol);
    _keys_current[currentcol] = (((*_port) & _mask) >> _first);
    _keys_old[currentcol] = _keys_current[currentcol];
  }
}


void KeyboardDynamic::delay_ms(int n)
{
  while (n--);
}

void KeyboardDynamic::read()
{
  unsigned short int currentcol;
  unsigned short int i;
  unsigned short int deltakeys;

  for (currentcol = 0; currentcol < _colcount; currentcol++)
  {

      //(*_colclr) = _colmask;
      IO0CLR = _colmask;
      //(*_colset) = (1 << (currentcol + _colfirst));
      IO0SET = (1 << (currentcol + _colfirst));

      delay_ms(1000);

      //_keys_current[currentcol] = (((*_port) & _mask) >> _first);
      _keys_current[currentcol] = ((IO0PIN & _mask) >> _first);
    
      if (_keys_current[currentcol] != _keys_old[currentcol])
      {
        //debug_printf("KEY %d\n",keys_current);
        
        deltakeys = (_keys_current[currentcol] ^ _keys_old[currentcol]);

      //  debug_printf("DELTA %X\n",deltakeys);
        for (i=0;i<_count;i++)
        {
          if (deltakeys & (1 << i))
          {
            KeyEvent event;
            if (_keys_current[currentcol] & (1 << i)) event.Make = 1;
            else event.Make = 0;
            event.Scancode = i | (currentcol << 8);
            _keybuffer.push(event);
          }
        }
        _keys_old[currentcol] = _keys_current[currentcol];
      }
    
  }
  //(*_colclr) = _colmask;
  IO0CLR = _colmask;
}


