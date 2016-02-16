
# include <stdio.h>
# include <string.h>
# include <libarm.h>

#include "common.h"

#include "lcd_hd44780.h"
#include "ringbuffer.h"
#include "keyboard_dynamic.h"
#include "uart.h"
#include "commandlinehandler.h"
#include "i2c_slave.h"

#include "cnokialcd.h"

#include "rectangle.h"
#include "font.h"
#include "bitmap.h"

#define DEMO_LED (1 << 30)

Bitmap * bmp;
Font * comicfont; 
Rectangle * temprectangle;
CNokiaLCD * nokialcd;
int menustatus = 0;
/*********************************************************************/
/* INTERRUPT TIMER0                                                  */
/*********************************************************************/

volatile unsigned short int timeout10msec = 0;
volatile unsigned short int timer400msec = 40;
volatile unsigned short int timeout400msec = 0;

extern "C" static void timer0ISR(void) __attribute__ ((interrupt ("IRQ")));

extern "C" static void timer0ISR(void)
{
   timeout10msec = 1;

   if (!(--timer400msec)) {
    timer400msec = 40;
    timeout400msec = 1;
   }

// VICSoftIntClear = 0xFFFFFFFF;  /* Clear the timer 0 interrupt */
  T0IR = 0xFF;
  /* Update VIC priorities */
  VICVectAddr = 0;
}



/*********************************************************************/
/* DEFAULT INTERRUPT                                                 */
/*********************************************************************/
int defaultOccurred = 0;

extern "C" static void defaultISR(void) __attribute__ ((interrupt ("IRQ")));

extern "C" static void defaultISR(void)
{
  defaultOccurred = 1;

  /* Update VIC priorities */
  VICVectAddr = 0;
}

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
/*
void delay_ms(unsigned long ms)
{
  delay_ticks((ticks_per_second() / 1000) * ms);
}
*/
void delay_us(unsigned long us)
{
  delay_ticks((ticks_per_second() / 1000000) * us);
}

void delay_ms(int n)
{
  while (n--);
}


/*********************************************************************/
/* TEMPERATURE                                                       */
/*********************************************************************/
#define SE95SLA 0x4F

void display_temperature()
{
  char tempStr[10];
  char temp_msb;
  char temp_lsb;
  short int temp = 0;
  float ftemp;

  i2cSlave * thermometer = new i2cSlave(0,SE95SLA);

  thermometer->init();

  if (thermometer->setStart())
  {
    if (thermometer->setSLAW())
    {
      thermometer->setData(0x0);
      thermometer->setRestart();
      thermometer->setSLAR();
      temp_msb = thermometer->readByte();
      temp_lsb = thermometer->readLastByte();

      if (thermometer->status() == I2C_STATUS_RECEIVE_NACK)
      {
        temp = (temp_msb << 8) | temp_lsb;

        ftemp = (temp >> 5) * 0.125;
        sprintf(tempStr,"%3.1f °C",ftemp);
    
        bmp->Clear(0xFFFF);
        bmp->DrawText(comicfont,temprectangle,tempStr,0x00);
        nokialcd->PaintDSBitmap(bmp->_image_buffer,82,114,60,18);
      }

    } 
    thermometer->setStop();
  }

  delete thermometer;
}

void PrintKeyText(char * string)
{
  bmp->Clear(0xFFFF);
  bmp->DrawText(comicfont,temprectangle,string,0x00);
  nokialcd->PaintDSBitmap(bmp->_image_buffer,82,1,60,18);
}

#define MENU_DY 14
Rectangle * menurectangle;
void PrintMenu(unsigned char active)
{
  
  bmp->Clear(0x00FF);
  bmp->DrawText(comicfont,menurectangle,"Menu",0x00);
  nokialcd->PaintDSBitmap(bmp->_image_buffer,30,10+(MENU_DY*1),60,18);

  if (active == 1) {
    bmp->Clear(0x0FFF);
    bmp->DrawText(comicfont,menurectangle,"Single",0x000);
  } else {
    bmp->Clear(0x0000);
    bmp->DrawText(comicfont,menurectangle,"Single",0xFFF);
  }
  nokialcd->PaintDSBitmap(bmp->_image_buffer,30,10+(MENU_DY*2),60,18);

  if (active == 2) {
    bmp->Clear(0x0FFF);
    bmp->DrawText(comicfont,menurectangle,"History",0x000);
  } else {
    bmp->Clear(0x0000);
    bmp->DrawText(comicfont,menurectangle,"History",0xFFF);
  }
  nokialcd->PaintDSBitmap(bmp->_image_buffer,30,10+(MENU_DY*3),60,18);

  if (active == 3) {
    bmp->Clear(0x0FFF);
    bmp->DrawText(comicfont,menurectangle,"Settings",0x000);
  } else {
    bmp->Clear(0x0000);
    bmp->DrawText(comicfont,menurectangle,"Settings",0xFFF);
  }
  nokialcd->PaintDSBitmap(bmp->_image_buffer,30,10+(MENU_DY*4),60,18);

  if (active == 4) {
    bmp->Clear(0x0FFF);
    bmp->DrawText(comicfont,menurectangle,"Exit",0x000);
  } else {
    bmp->Clear(0x0000);
    bmp->DrawText(comicfont,temprectangle,"Exit",0xFFF);
  }
  nokialcd->PaintDSBitmap(bmp->_image_buffer,30,10+(MENU_DY*5),60,18);
}

/*********************************************************************/
/* SETUP                                                             */
/*********************************************************************/
void init()
{

  /************************************************/
  /* DEFAULT                                      */
  /************************************************/
  VICDefVectAddr = (unsigned int)defaultISR;

  /************************************************/
  /*  2  TIMER0                                   */
  /************************************************/

  T0TCR = 2;
  T0TCR = 0; /* Reset timer 0 */

  T0PR = 252; // 10MHz: 20MHz: 504 / 14. MHz: 370 /* Set the timer 0 prescale counter */
  T0MR0 = 989; // 20/14. MHz: 989 /* Set timer 0 match register */
  //T0PR = liblpc2000_get_pclk(liblpc2000_get_cclk(OSCILLATOR_CLOCK_FREQUENCY)) / ticks_per_second();
  T0MCR = 3; /* Generate interrupt and reset counter on match */

  T0TCR = 1; /* Start timer 0 */

  /************************************************/
  /*  2  TIMER0                                   */
  /************************************************/
  VICIntSelect &= ~0x10; /* Timer 0 interrupt is IRQ interrupt */
  VICIntEnable = 0x10; /* Enable timer 0 interrupt */
  VICVectCntl2 = 0x20 | 0x4; /* Enabled (Bit 5) Use slot for timer 0 interrupt (0x4) */
  VICVectAddr2 = (unsigned int)timer0ISR; /* Set the address of ISR for slot 0 */
  
  libarm_enable_irq(); 
  

}

/*
#define SCANCODE_UP 0
#define SCANCODE_DOWN 1
#define SCANCODE_LEFT 2
#define SCANCODE_RIGHT 3
#define SCANCODE_ESC 4
#define SCANCODE_OK 6
*/

#define SCANCODE_NUM_1 0x100
#define SCANCODE_NUM_2 0x101
#define SCANCODE_NUM_3 0x102
#define SCANCODE_OK 0x103

#define SCANCODE_NUM_4 0x200
#define SCANCODE_NUM_5 0x201
#define SCANCODE_NUM_6 0x202
#define SCANCODE_UP 0x203

#define SCANCODE_NUM_7 0x300
#define SCANCODE_NUM_8 0x301
#define SCANCODE_NUM_9 0x302
#define SCANCODE_DOWN 0x303

#define SCANCODE_ESC 0x0
#define SCANCODE_NUM_0 0x1
#define SCANCODE_LEFT 0x2
#define SCANCODE_RIGHT 0x3





//**********************************************************
// FILE HANDLING
//**********************************************************

EmbeddedFileSystem  efs;
EmbeddedFile        file;

void format_file_name(char *dest, char *src)
{
  char i,k;

    for (i=7; i>0 && (src[i] == 0x20); i--);

    strncpy(dest, src, i+1);
    dest[i+1] = '.';

    for (k=2; k>0 && (src[8+k] == 0x20); k--);

    strncpy(&dest[i+2], &src[8], k+1);
    dest[i+5] = '\0';
}

int FindFiles()
{

 DirList             list;
 char       file_name[13];
 char       file_path[23];
 unsigned int        size;
 int width = 96;
 int height = 120;
 int x = 10;
 int y = 5;

 int numberoffiles = 0;
 char lcdtext[9];

 unsigned short i, e ;
 char buf [ 512 ] ;
 
  if (ls_openDir(&list, &(efs.myFs), "/0/") != 0)
  {
    DBG((TXT("Could not open the selected directory.\n")));
    return(-2);
  }

  
  while (ls_getNext(&list) == 0)
  {
    numberoffiles++;
    if  ( (list.currentEntry.FileName[8] == '4') &&
	  (list.currentEntry.FileName[9] == 'B') &&
          (list.currentEntry.FileName[10] == 'I') )
      {
        format_file_name(file_name, (char*)list.currentEntry.FileName);

        sprintf(file_path,"/0/%s",file_name);

       // DBG((TXT("Filename: %.11s (%li bytes)\n"), list.currentEntry.FileName, list.currentEntry.FileSize));


	if(file_fopen(&file,&efs.myFs,(eint8 *)file_path,'r')==0)
	{

          // Column address set (command 0x2A)
          nokialcd->WriteSpiCommand(CASET);
          nokialcd->WriteSpiData(x);
          nokialcd->WriteSpiData(x + width - 1);
          
          // Page address set (command 0x2B)
          nokialcd->WriteSpiCommand(PASET);
          nokialcd->WriteSpiData(y);
          nokialcd->WriteSpiData(y + height - 1);
          
          // WRITE MEMORY
          nokialcd->WriteSpiCommand(RAMWR);

          while ( e = file_read (&file , 512 , (euint8 *)buf ) ) 
          {
            for ( i = 0; i<e ; i++) nokialcd->WriteSpiData(buf[i]);
          }

          //LCDPutStr(file_name, 10, 80, LARGE, WHITE, BLACK);

          file_fclose(&file);
          display_temperature();
	}
	else
          DBG((TXT("Could not open file.\n")));

    }
  }

  
  //debug_printf("%d FILES\n",numberoffiles);
   
}


int sd_mount()
{
 if(efs_init(&efs,"\\")!=0)
  {
    DBG((TXT("Could not open filesystem.\n")));
    return(-1);
  }


 
}

void sd_unmount()
{
  fs_umount(&(efs.myFs));
}


LCD_HD44780 * lcd = new LCD_HD44780();
LCD_HD44780 lcd2;
int main ()
{
  
  int i,j;
  
  
  // KeyboardStatic * keyboard = new KeyboardStatic(&IO0PIN,21,7);


  UARTHandler * commandline = new UARTHandler();

  PINSEL0 = 0x00;
  PINSEL1 = 0x00;

  // PCONP = PCONP_PCSPI0 | PCONP_PCTIM0 | PCONP_PCEMC;

  IO0DIR = DEMO_LED;

  //PCONP |= PCONP_PCSPI1;
  IO0DIR  |= (BIT15 | BIT16 | BIT8 | BIT21 | BIT22 | BIT23 | BIT24);
  
  SPIPort * spi1 = new SPIPort(1);

  spi1->On();

  nokialcd = new CNokiaLCD(spi1);

  KeyboardDynamic * keyboard = new KeyboardDynamic(&IO0PIN,10,4,&IO0PIN,&IO0CLR,&IO0SET,21,4);
  
  bmp = new Bitmap(60,18);
  
  comicfont = new Font(FontTypeComicSans10);
  temprectangle = new Rectangle(1,1,59,16);
  menurectangle = new Rectangle(0,0,59,15);

  nokialcd->Init();
  
  nokialcd->Backlight(1);

  nokialcd->ClearScreen();

  nokialcd->TestPattern();

  delay_ms(1000);

  // ***************************************************************
  // * Display resident Pinky
  // ***************************************************************
  
  nokialcd->SetRect(0, 0, 131, 131, FILL, WHITE);  
  nokialcd->WriteBitmap(0,5,96,120,0);

  init();
  
  lcd->Init();

  lcd->GotoXY(0,0);
  lcd->Print("BUZZ");
  
  lcd->GotoXY(0,1);
  lcd->Print("YUKI");
/*
  if (sd_mount() == 0)
  {
    //while (1) 
    //{
      //FindFiles();
     
    //}
  }
*/
  while (1)
  {

   if (keyboard->bufferLength() != 0)
   {
      KeyEvent key = keyboard->bufferPop();
      lcd->GotoXY(0,0);
      //debug_printf("SCAN: %X  MAKE: %X \n",key.Scancode,key.Make);
      if (key.Make) 
      {
        if (key.Scancode == SCANCODE_OK)
        {
          if (menustatus)
           if (menustatus == 4) 
           {
            menustatus = 0;
            nokialcd->WriteBitmap(0,5,96,120,0);
           }
        }
        else if (key.Scancode == SCANCODE_ESC)   {
          if (!menustatus)
          {
            menustatus = 1;
            PrintMenu(menustatus);
          } else {
            menustatus = 0;
            nokialcd->WriteBitmap(0,5,96,120,0);
          }
        }
        else if (key.Scancode == SCANCODE_UP)   {
          if (menustatus)
           if (menustatus > 1) {
            menustatus--;
            PrintMenu(menustatus);
           }
        }
        else if (key.Scancode == SCANCODE_DOWN)  {
          if (menustatus)
           if (menustatus < 4) {
            menustatus++;
            PrintMenu(menustatus);
           }
        }
        if (key.Scancode == SCANCODE_LEFT)  PrintKeyText("LEFT");
        if (key.Scancode == SCANCODE_RIGHT) PrintKeyText("RIGHT");
      } else {
                                            PrintKeyText("");
      }

   }

   if (timeout400msec)
   {
      timeout400msec = 0;
      if ((IO0PIN & DEMO_LED) == DEMO_LED) 
      {
        IO0CLR = DEMO_LED;  
      }
      else 
      {
        IO0SET = DEMO_LED;  
        display_temperature();
      }
   }

   if (timeout10msec)
   {
      timeout10msec = 0;
      keyboard->read();
   }
   

  }

}

