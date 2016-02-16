
# include <stdio.h>
# include <string.h>

# include <targets/lpc2294.h>


#include <lcd_HD44780.h>

void bin_prnt_byte(char t, char x)
{
/*
   int n;
   int y;
   y = x;
   char txt [10] = "00000000";

   for(n=0; n<8; n++)
   {
      if((x & 0x80) !=0)
      {
         txt[n] = '1';
      }
      else
      {
         txt[n] = '0';
      }
      x = x<<1;
   }
   debug_printf("%c %02X 0b%s\n",t,y,txt);
  */
}


void Delay(int count)
{
  int i;
  while(count--)
  {
    i = 1000;
    while (i--) { }
  }
}

void SDelay()
{
  int i = 10;
  while (i--) { }
}

void LCD_BG_On()
{
  LCDSET = LCD_BG_POWER;
}

void LCD_BG_Off()
{
  LCDCLR = LCD_BG_POWER;
}

void LCD_Push()
{
  LCDSET = LCD_E;
  SDelay();
  LCDCLR = LCD_E;
}

void LCD_SetData(char data)
{
  LCDCLR =  (0x0F << 21);
  LCDSET = ((data & 0x0F) << 21);
}

char LCD_Read()
{
  char high, low, data;

  LCDDIR &= ~LCD_DATA;
  LCDCLR = LCD_RS;
  LCDSET = LCD_RW;
  LCDSET = LCD_E;
  SDelay();
  high = ((IO1PIN & LCD_DATA) >> 21);
  LCDCLR = LCD_E;
  SDelay();
  LCDSET = LCD_E;
  SDelay();
  low = ((IO1PIN & LCD_DATA) >> 21);
  LCDCLR = LCD_E;
  LCDDIR |=  LCD_DATA;
  data = ((high << 4) | low);
  bin_prnt_byte('R',data);
  return data;
}


char LCD_WriteData(char data)
{
  char status = 0x80;

  while (status & 0x80)
  {
    status = LCD_Read();
  }

  bin_prnt_byte('D',data);

  LCDSET = LCD_RS;
  LCDCLR = LCD_RW;

  // 6800
  LCD_SetData(data >> 4);  
  LCD_Push();
  LCD_SetData(data & 0x0F);  
  LCD_Push();

}

char LCD_WriteCommand(char data)
{
  char status = 0x80;

  while (status & 0x80)
  {
    status = LCD_Read();
  }

  bin_prnt_byte('C',data);

  LCDCLR = LCD_RS | LCD_RW;

  LCD_SetData(data >> 4);  
  LCD_Push();
  LCD_SetData(data & 0x0F);  
  LCD_Push();
  
}

void LCD_Print(char * str)
{
  char c;
  while (c = *str++) LCD_WriteData(c);
}

void LCD_Clear()
{
  LCD_WriteCommand(0x1);
}

void LCD_Home()
{
  LCD_WriteCommand(0x2);
}

void LCD_SetEntryMode(char increase, char displayshift)
{
   LCD_WriteCommand(0x4 | (increase << 1) | (displayshift) );
}

void LCD_SetDisplayMode(char displayon, char cursoron, char cursorblink)
{
   LCD_WriteCommand(0x8 | (displayon << 2) | (cursoron << 1) | (cursorblink) );
}

void LCD_MoveCursor(char directionright)
{
   LCD_WriteCommand(0x10 | (directionright << 2));
}

void LCD_ShiftDisplay(char directionleft)
{
   LCD_WriteCommand(0x10 | (1 << 3) | (directionleft << 2));
}

void LCD_SetFunction(char interface8bit, char twolines, char font5x10)
{
   LCD_WriteCommand(0x20 | (interface8bit << 4) | (twolines << 3) | (font5x10 << 2) );
}

void LCD_SetCGRAM(int address)
{
   LCD_WriteCommand(0x40 | (address) );
}

void LCD_SetDDRAM(int address)
{
   LCD_WriteCommand(0x80 | (address) );
}

void LCD_Init4Bit()
{
  /*
  // INIT BY COMMAND 
  // May be needed if internal reset circuit of HD44780 does not work right
  LCD_SetData(0x3); // INIT
  LCD_Push();
  Delay(40);
  LCD_Push();
  Delay(1);
  LCD_Push();
  Delay(1);
  */

  LCD_SetData(0x2); // 4-BIT
  LCD_Push();
  Delay(1);
}

void LCD_GotoXY(char x, char y)
{
  LCD_SetDDRAM((y << 6) + x);
}

void LCD_Init()
{
  LCDDIR |= LCD_BG_POWER | LCD_RS | LCD_E | LCD_RW | LCD_DATA;
  
  LCD_BG_On();

  LCD_Init4Bit();
 
  LCD_SetFunction(0,1,0); // 4-BIT, 2-LINE, 5X7 DOTS
  LCD_SetDisplayMode(1,0,0); // DISPLAY ON, CURSOR OFF, BLINKING OFF
  LCD_SetEntryMode(1,0); // DIRECTION RIGHT, NO DISPLAY SHIFT

  LCD_Clear();
}

