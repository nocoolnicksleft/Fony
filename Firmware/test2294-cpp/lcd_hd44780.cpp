

#include "lcd_hd44780.h"
  


LCD_HD44780::LCD_HD44780()
{

}

LCD_HD44780::~LCD_HD44780()
{

}


void LCD_HD44780::bin_prnt_byte(char t, char x)
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


void LCD_HD44780::Delay(int count)
{
  int i;
  while(count--)
  {
    i = 1000;
    while (i--) { }
  }
}

void LCD_HD44780::SDelay()
{
  int i = 10;
  while (i--) { }
}

void LCD_HD44780::BG_On()
{
  LCDSET = LCD_BG_POWER;
}

void LCD_HD44780::BG_Off()
{
  LCDCLR = LCD_BG_POWER;
}

char LCD_HD44780::IsBGOn()
{
 if (LCDPIN & LCD_BG_POWER) return 1;
 else return 0;
}

void LCD_HD44780::Push()
{
  LCDSET = LCD_E;
  SDelay();
  LCDCLR = LCD_E;
}

void LCD_HD44780::SetData(char data)
{
  LCDCLR =  (0x0F << 21);
  LCDSET = ((data & 0x0F) << 21);
}

char LCD_HD44780::Read()
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


char LCD_HD44780::WriteData(char data)
{
  char status = 0x80;

  while (status & 0x80)
  {
    status = this->Read();
  }

  this->bin_prnt_byte('D',data);

  LCDSET = LCD_RS;
  LCDCLR = LCD_RW;

  // 6800
  this->SetData(data >> 4);  
  this->Push();
  this->SetData(data & 0x0F);  
  this->Push();

}

char LCD_HD44780::WriteCommand(char data)
{
  char status = 0x80;

  while (status & 0x80)
  {
    status = this->Read();
  }

  this->bin_prnt_byte('C',data);

  LCDCLR = LCD_RS | LCD_RW;

  this->SetData(data >> 4);  
  this->Push();
  this->SetData(data & 0x0F);  
  this->Push();
  
}

void LCD_HD44780::Print(char * str)
{
  char c;
  while (c = *str++) this->WriteData(c);
}

void LCD_HD44780::Clear()
{
  this->WriteCommand(0x1);
}

void LCD_HD44780::Home()
{
  this->WriteCommand(0x2);
}

void LCD_HD44780::SetEntryMode(char increase, char displayshift)
{
   this->WriteCommand(0x4 | (increase << 1) | (displayshift) );
}

void LCD_HD44780::SetDisplayMode(char displayon, char cursoron, char cursorblink)
{
   this->WriteCommand(0x8 | (displayon << 2) | (cursoron << 1) | (cursorblink) );
}

void LCD_HD44780::MoveCursor(char directionright)
{
   this->WriteCommand(0x10 | (directionright << 2));
}

void LCD_HD44780::ShiftDisplay(char directionleft)
{
   this->WriteCommand(0x10 | (1 << 3) | (directionleft << 2));
}

void LCD_HD44780::SetFunction(char interface8bit, char twolines, char font5x10)
{
   this->WriteCommand(0x20 | (interface8bit << 4) | (twolines << 3) | (font5x10 << 2) );
}

void LCD_HD44780::SetCGRAM(int address)
{
   this->WriteCommand(0x40 | (address) );
}

void LCD_HD44780::SetDDRAM(int address)
{
   this->WriteCommand(0x80 | (address) );
}

void LCD_HD44780::Init4Bit()
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

  this->SetData(0x2); // 4-BIT
  this->Push();
  this->Delay(1);
}

void LCD_HD44780::GotoXY(char x, char y)
{
  this->SetDDRAM((y << 6) + x);
}

void LCD_HD44780::Init()
{
  LCDDIR |= LCD_BG_POWER | LCD_RS | LCD_E | LCD_RW | LCD_DATA;
  
  this->BG_On();

  this->Init4Bit();
 
  this->SetFunction(0,1,0); // 4-BIT, 2-LINE, 5X7 DOTS
  this->SetDisplayMode(1,0,0); // DISPLAY ON, CURSOR OFF, BLINKING OFF
  this->SetEntryMode(1,0); // DIRECTION RIGHT, NO DISPLAY SHIFT

  this->Clear();
}


