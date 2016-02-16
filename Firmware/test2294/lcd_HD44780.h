
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



