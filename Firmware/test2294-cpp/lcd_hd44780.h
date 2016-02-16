
#ifndef _LCD_HD44780_H_
#define _LCD_HD44780_H_

#define LCDSET IO1SET
#define LCDCLR IO1CLR
#define LCDPIN IO1PIN
#define LCDDIR IO1DIR

#define LCD_BG_POWER (1 << 16)
#define LCD_RS (1 << 17)
#define LCD_E (1 << 18)
#define LCD_RW (1 << 19)

#define LCD_D4 (1 << 21)
#define LCD_D5 (1 << 22)
#define LCD_D6 (1 << 23)
#define LCD_D7 (1 << 24)
#define LCD_DATA (LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7)

# include <stdio.h>
# include <string.h>

# include <targets/lpc2294.h>


class LCD_HD44780
{

  public:
	LCD_HD44780(void);

  public:
	~LCD_HD44780(void);

  protected:
        void bin_prnt_byte(char t, char x);;
        void Delay(int count);
        void SDelay();
        void Push();
        void SetData(char data);
        void Init4Bit();

  public:
        void BG_On();
        void BG_Off();
        char Read();
        char WriteData(char data);
        char WriteCommand(char data);
        void Print(char * str);
        void Clear();
        void Home();
        void SetEntryMode(char increase, char displayshift);
        void SetDisplayMode(char displayon, char cursoron, char cursorblink);
        void MoveCursor(char directionright);
        void ShiftDisplay(char directionleft);
        void SetFunction(char interface8bit, char twolines, char font5x10);
        void SetCGRAM(int address);
        void SetDDRAM(int address);
        void GotoXY(char x, char y);
        void Init();
        char IsBGOn();

};

#endif
