


#define LCD_CS_HIGH()			IO0SET = BIT16
#define LCD_CS_LOW()			IO0CLR = BIT16

#define LCD_RESET_HIGH()		IO0SET = BIT15
#define LCD_RESET_LOW()			IO0CLR = BIT15

#define LCD_DATA_HIGH()                 IO0SET = BIT19
#define LCD_DATA_LOW()			IO0CLR = BIT19

#define LCD_CLK_HIGH()                  IO0SET = BIT17
#define LCD_CLK_LOW()			IO0CLR = BIT17

#define LCDCommand      0
#define LCDData         1

#define LOW_RES         0x01                            // 8-bit R-G-B
#define HIGH_RES        0x02                            // 16-bit R-G-B 


// 8bpp Color Mode - Some basic colors defined for ease of use
// remember 8bpp color = 3xRed, 3xGreen & 2xBlue bits
// organised as RRRGGGBB

#define DarkBlue    0x02
#define LightBlue   0x03
#define DarkGreen   0x14
#define LightGreen  0x1c
#define DarkRed     0x60
#define LightRed    0xe0
#define White       0xff
#define Black       0x00
#define Yellow      0xdd 
#define Purple      0x62


#define DISON       0xaf   
#define DISOFF      0xae   
#define DISNOR      0xa6  
#define DISINV      0xa7  
#define COMSCN      0xbb   
#define DISCTL      0xca   
#define SLPIN       0x95   
#define SLPOUT      0x94   
#define PASET       0x75   
#define CASET       0x15   
#define DATCTL      0xbc   
#define RGBSET8     0xce   
#define RAMWR       0x5c   
#define RAMRD       0x5d   
#define PTLIN       0xa8   
#define PTLOUT      0xa9   
#define RMWIN       0xe0   
#define RMWOUT      0xee   
#define ASCSET      0xaa   
#define SCSTART     0xab   
#define OSCON       0xd1   
#define OSCOFF      0xd2   
#define PWRCTR      0x20   
#define VOLCTR      0x81   
#define VOLUP       0xd6   
#define VOLDOWN     0xd7   
#define TMPGRD      0x82   
#define EPCTIN      0xcd   
#define EPCOUT      0xcc   
#define EPMWR       0xfc   
#define EPMRD       0xfd   
#define EPSRRD1     0x7c   
#define EPSRRD2     0x7d   
#define NOP         0x25  

void SendLcd(unsigned char type, unsigned int data);
void InitLCD(void);
void GotoXY(unsigned char x, unsigned char y);
void LCD_PixelPut(unsigned char x, unsigned char y, unsigned char color);
void LCD_Fill (unsigned char xs,unsigned char ys,unsigned char width,unsigned char height, unsigned char color);
void LCD_Box(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color);
void Character (unsigned char Size, unsigned char x, unsigned char y, unsigned char _char, unsigned char color);
void LCD_Line(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color);
void LCD_Funct_B(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color );
void LCD_Funct_A(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color );
void LCD_Circle(unsigned char center_x, unsigned char center_y, unsigned int rad, unsigned char color);
void LCD_Thick_Circle(unsigned char center_x, unsigned char center_y,unsigned char diameter, unsigned char thick,unsigned char color);
void LCD_Circle_Line(unsigned char center_x, unsigned char center_y,unsigned char diameter, unsigned char length, float angle,unsigned char lcolor);
void ShowImage(unsigned char x, unsigned char y, const unsigned char *location);
void ShowImage_4096(unsigned char x,unsigned char y, const unsigned char  *location);
void LCD_String (char *lcd_string, const char *font_style,unsigned char x, unsigned char y, unsigned char fcolor, unsigned char bcolor);
void LCD_Set_Resolution(char res);


