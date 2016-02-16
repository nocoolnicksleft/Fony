
#define VECTORED_IRQ_INTERRUPTS

# include <stdio.h>
# include <string.h>

# include <targets/lpc2294.h>

# include <lcd_HD44780.h>

# include <efs.h>
# include <ls.h>
# include "lpc_io.h"
# include "debug.h"



EmbeddedFileSystem  efs;
EmbeddedFile        file;
DirList             list;
unsigned char       file_name[13];
unsigned int        size;



// GPIO LINES PORT 0

#define DEMO_LED (1 << 30)

#define SD_SCK (1 << 4)
#define SD_MISO (1 << 5)
#define SD_MOSI (1 << 6)
#define SD_CS (1 << 9)
#define SD_ENABLED (1 << 10)
#define SD_PRESENT (1 << 11)
#define SD_ACTIVITY (1 << 12)
#define SD_POWER (1 << 13)

#define KEYPIN IO0PIN
#define KEYDIR IO0DIR
#define NUMKEYS 6
#define KEY_UP (1 << 21)
#define KEY_DOWN (1 << 22)
#define KEY_RIGHT (1 << 23)
#define KEY_LEFT (1 << 24)
#define KEY_ESCAPE (1 << 25)
#define KEY_OK (1 << 26)
#define KEY_ALL ( KEY_UP | KEY_DOWN | KEY_RIGHT | KEY_LEFT | KEY_ESCAPE | KEY_OK)

#define KBD_MAKE 0x40
#define KBD_BREAK 0x80

//**********************************************************
// KEYBOARD HANDLING
//**********************************************************
#define QUEUE_LENGTH 30
char queue[QUEUE_LENGTH];
int queue_start = 0;
int queue_stop = 0;
int popone = 0;

int kbd_queue_length()
{
  if (queue_start == queue_stop) return 0;
  if (queue_start < queue_stop) return  (queue_stop - queue_start);
  else return  (QUEUE_LENGTH-queue_start) + queue_stop;
}

void kbd_queue_push(char c)
{
// fprintf(terminal,"push %c qlen: %u qstart: %u qstop: %u \r\n",c,get_queue_length(),queue_start,queue_stop);
 queue[queue_stop] = c;
 if (queue_stop == (QUEUE_LENGTH-1)) {
   if (queue_start>0) {
    queue_stop = 0;
    popone = 1;
   }
 } else {
  if (queue_stop == (queue_start - 1)) {
  } else {
   queue_stop++;
   popone = 1;
  }
 } 
}

char kbd_queue_pop()
{
 char c = 0;
 if (queue_start != queue_stop) {
  c = queue[queue_start];
//  fprintf(terminal,"pop %c qstart: %u qstop: %u \r\n",c,queue_start,queue_stop);
  if (queue_start == (QUEUE_LENGTH-1)) queue_start = 0;
  else {
    queue_start++;
  }
 }
 return c;
}



void kbd_init()
{
  KEYDIR &= ~(KEY_ALL);
}

void kbd_read()
{
  static int keys_old = 0x3F;
  static int keys_current = 0;
  keys_current = ((KEYPIN & KEY_ALL) >> 21);
  if (keys_current != keys_old)
  {
    debug_printf("KEY %d\n",keys_current);
    int i;
    int deltakeys = (keys_current ^ keys_old);
    for (i=0;i<NUMKEYS;i++)
    {
      if (deltakeys & (1 << i))
      {
        if (keys_current & (1 << i)) kbd_queue_push(KBD_BREAK | i );
        else kbd_queue_push(KBD_MAKE | i);
      }
    }
    keys_old = keys_current;
  }
}


/*********************************************************************/
/* DEFAULT INTERRUPT                                                 */
/*********************************************************************/
int defaultOccurred = 0;

static void defaultISR(void) __attribute__ ((interrupt ("IRQ")));

static void defaultISR(void)
{
  defaultOccurred = 1;

  /* Update VIC priorities */
  VICVectAddr = 0;
}

/*********************************************************************/
/* INTERRUPT TIMER0 (FIQ)                                            */
/*********************************************************************/

volatile unsigned short int timeout10msec = 0;
volatile unsigned short int timer400msec = 40;
volatile unsigned short int timeout400msec = 0;


static void timer0ISR(void) __attribute__ ((interrupt ("IRQ")));

static void timer0ISR(void)
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

//**********************************************************
// FILE HANDLING
//**********************************************************
void format_file_name(unsigned char *dest, unsigned char *src)
{
  unsigned char i,k;

    for (i=7; i>0 && (src[i] == 0x20); i--);

    strncpy(dest, src, i+1);
    dest[i+1] = '.';

    for (k=2; k>0 && (src[8+k] == 0x20); k--);

    strncpy(&dest[i+2], &src[8], k+1);
    dest[i+5] = '\0';
}


int FindFiles()
{
 int numberoffiles = 0;
 char lcdtext[9];
 
  LCD_GotoXY(0,1); // SET CURSOR TO SECOND LINE
  LCD_Print("FS OPEN");

  if(efs_init(&efs,"\\")!=0)
  {
    DBG((TXT("Could not open filesystem.\n")));
    return(-1);
  }

  LCD_GotoXY(0,1); // SET CURSOR TO SECOND LINE
  LCD_Print("DIR OPEN");

  if (ls_openDir(&list, &(efs.myFs), "/0/") != 0)
  {
    DBG((TXT("Could not open the selected directory.\n")));
    return(-2);
  }

  LCD_GotoXY(0,1); // SET CURSOR TO SECOND LINE
  LCD_Print("FIL OPEN");

  while (ls_getNext(&list) == 0)
  {
    numberoffiles++;
    if  ( (list.currentEntry.FileName[8] == 'M') &&
	  (list.currentEntry.FileName[9] == 'P') &&
          (list.currentEntry.FileName[10] == '3') )
      {
        DBG((TXT("Filename: %.11s (%li bytes)\n"), list.currentEntry.FileName,
                                                   list.currentEntry.FileSize));

        format_file_name(file_name, list.currentEntry.FileName);

	if(file_fopen(&file,&efs.myFs,file_name,'r')==0)
	{
          file_fclose(&file);
	}
	else
          DBG((TXT("Could not open file.\n")));

    } /* if */
  } /* while */

  fs_umount(&(efs.myFs));
  
  LCD_Clear();
  LCD_GotoXY(0,0);
  sprintf(lcdtext,"%d FILES",numberoffiles);
  LCD_Print(lcdtext);
   
}


int main()
{

 // set pin functions for port 0
 PINSEL1 = 0x00;

 // set outbound gpio for port 0
 IO0DIR = DEMO_LED | SD_SCK | SD_MOSI | SD_CS | SD_ACTIVITY | SD_POWER;
 
 // set pin functions for port 1
 PINSEL1 = 0x00;

 // set outbound gpio for port 1
 //IO1DIR = LCD_BG_POWER | LCD_RS | LCD_E | LCD_RW | LCD_DATA;


  /************************************************/
  /* DEFAULT                                      */
  /************************************************/
  VICDefVectAddr = (unsigned int)defaultISR;

  /************************************************/
  /*  2  TIMER0                                   */
  /************************************************/

  T0TCR = 0; /* Reset timer 0 */
  T0PR = 252; // 10MHz: 20MHz: 504 / 14. MHz: 370 /* Set the timer 0 prescale counter */
  T0MR0 = 989; // 20/14. MHz: 989 /* Set timer 0 match register */
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

  Delay(1000);

  LCD_Init();

  LCD_Print("INIT");
  LCD_GotoXY(0,1); // SET CURSOR TO SECOND LINE
  LCD_Print("FW 0.01");

  int x = 0;

  init_IO();

  FindFiles();

 while(1)
 {

   if (timeout400msec)
   {
    timeout400msec = 0;
    if (IO0PIN & DEMO_LED) IO0CLR = DEMO_LED;  
    else IO0SET = DEMO_LED;  

    LCD_GotoXY(x,1);
    LCD_Print(" ");

    if (x == 7) x = 0;
    else x++;

    LCD_GotoXY(x,1);
    LCD_Print("*");

   }

   if (timeout10msec)
   {
    kbd_read();
    timeout10msec = 0;
   }

   if (kbd_queue_length())
   {
      char key = kbd_queue_pop();
      if (key & KBD_MAKE) 
      {
        char text[9];
        sprintf(text,"MAKE   %d",(key &= ~KBD_MAKE));
        debug_printf("MAKE   %d\n",(key &= ~KBD_MAKE));
        LCD_GotoXY(0,0);
        LCD_Print(text);
      } else {
        char text[9];
        sprintf(text,"BREAK  %d",(key &= ~KBD_BREAK));
        debug_printf("BREAK  %d\n",(key &= ~KBD_BREAK));
        LCD_GotoXY(0,0);
        LCD_Print(text);
      }
   }


   
 }

}

