#include <targets/lpc2294.h>
#include <math.h>
#include "pcf8833u8_lcd.h"
#include "fonts.h"
#include "bitmap.h"
#include "defines.h"


//*****************************************************************************
//	Name: 		SendLcd(u_char type, u_char dat) 
// 	Copyright:	Free to use at will & at own risk.
// 	Author: 	JCP
// 	Date: 		05.30.06 17:10
// 	Description:    Sends 9 bit SPI data to LCD
// 	Useage:		SendLcd(param 1,param 2)
//			param 1: Type of data, I.E. Data or Command
//			param 2: The data to be sent 
//
//	Example:        SendLcd(LCDCommand,0x00);         
//				or
//			SendLcd(LCDData,0x00);
//*******************************************************************************
void SendLcd(unsigned char type, unsigned int data) 
{

    while((S1SPSR & S1SPSR_SPIF) == 0);                  // if still busy wait here

    PINSEL1 &= ~(0x3 << 2); // SPI AUS

    if(type)
    {
        //data |=0x0100;                                      //set the 9'th bit
      LCD_DATA_HIGH(); 
    } else {
      LCD_DATA_LOW();
    }

    LCD_CLK_HIGH();

    LCD_CLK_LOW();

    PINSEL1 |= (0x3 << 2); // SPI EIN

    S1SPDR = data;                                         // Send the data using 9 bits

	
}
//*****************************************************************************
//	Name: 		InitLCD(void)
// 	Copyright:	Free to use at will & at own risk.
// 	Author: 	Taken from SparkFun's Website, ARM Example.
// 	Date: 		05.30.06 17:10
// 	Description:    To Initalize the Philips PCF8833 LCD Controller
//      Note:		I am not convinced that this init routine is corect. but it works....
//                      i nabed this routine from Sprkfuns other example code, the only thing
//                      i added here was the resolution routine.
//*******************************************************************************
void InitLCD(void)
{
	char i;

    LCD_RESET_LOW();
    ctl_timeout_wait(ctl_get_current_time() + 50);

    LCD_RESET_HIGH();
    ctl_timeout_wait(ctl_get_current_time() + 50);

    SendLcd(LCDCommand,DISCTL);  // display control
    SendLcd(LCDData,0x00);
    SendLcd(LCDData,0x1f);
    SendLcd(LCDData,0x0b);

    SendLcd(LCDCommand,COMSCN);        // 
    SendLcd(LCDData,0x01);             // Scan 1-80
    
    SendLcd(LCDCommand,OSCON);         //
    
    SendLcd(LCDCommand,SLPOUT);        // Sleep out command (must be done before DISPON)
    

    SendLcd(LCDCommand,VOLCTR);  // electronic volume, this is kinda contrast/brightness
    SendLcd(LCDData,0x5B);      // change this to get brighter or darker start point then use volup/down to adjust
    SendLcd(LCDData,0x03);

    SendLcd(LCDCommand,TMPGRD);  // Temp Gradient
    SendLcd(LCDData,0x01);

 
    LCD_Set_Resolution(LOW_RES);

    SendLcd(LCDCommand,RGBSET8);   // setup color lookup table
// Setup the color lookup table by choosing evenly spaced color values 
// for a nice even spread. Blue only has 2 bits, so the spacing is wider
// but still spans 0x00 - 0x0f
// Bit organisation is RRRGGGBB

    SendLcd(LCDCommand,RGBSET8);         
    SendLcd(LCDData,0x00);                // RED
    SendLcd(LCDData,0x02);
    SendLcd(LCDData,0x04);
    SendLcd(LCDData,0x06);
    SendLcd(LCDData,0x08);
    SendLcd(LCDData,0x0a);
    SendLcd(LCDData,0x0c);
    SendLcd(LCDData,0x0f);
    SendLcd(LCDData,0x00);                // GREEN
    SendLcd(LCDData,0x02);
    SendLcd(LCDData,0x04);
    SendLcd(LCDData,0x06);
    SendLcd(LCDData,0x08);
    SendLcd(LCDData,0x0a);
    SendLcd(LCDData,0x0c);
    SendLcd(LCDData,0x0f);
    SendLcd(LCDData,0x00);                // BLUE
    SendLcd(LCDData,0x04);
    SendLcd(LCDData,0x09);
    SendLcd(LCDData,0x0f);

    SendLcd(LCDCommand,NOP);                                // Data Sheet recomends you send this from time to time

    SendLcd(LCDCommand,DISINV);

    LCD_Fill(0,0,132,132,Black);                            // clear screen
    ctl_timeout_wait(ctl_get_current_time() + 100);         // data sheet says to wait 100ms here

    SendLcd(LCDCommand,PWRCTR);                             // power ctrl
    SendLcd(LCDData,0x0f);                                  //everything on

    ctl_timeout_wait(ctl_get_current_time() + 100);         // data sheet says to wait 100ms here

    SendLcd(LCDCommand,NOP);                                // Data Sheet recomends you send this from time to time
    SendLcd(LCDCommand, DISON);                             // Turn Display ON



}

//*****************************************************************************
//	Name: 		LCD_Set_Resolution(char res)
// 	Copyright:	Free to use at will & at own risk.
// 	Author: 	JCP
// 	Date: 		05.30.06 17:10
// 	Description:    Sets display color resolution 
// 	Useage:		LCD_Set_Resolution(param 1)
//
//			param 1: LOW_RES, MED_RES, HIGH_RES
//
//	Example:	LCD_Set_Resolution(LOW_RES); 
//
//      NOTE:           the only function that works in med_res is ShowImage_4096()
//                      maybe a later version i will make the rest to work with 
//                      the higher color settings.
//*******************************************************************************
void LCD_Set_Resolution(char res)
{
    SendLcd(LCDCommand,DATCTL);     // datctl
    SendLcd(LCDData,0x00);          // Param 1  Normal Page address, Normal column, Scan Column dir.
    SendLcd(LCDData,0x04);          // param 2  Set for R-G-B order
    SendLcd(LCDData,res);           // param 3  8-bit or 16 bit ?
}
//*****************************************************************************
//	Name: 		GotoXY(unsigned char x, unsigned char y)
// 	Copyright:	Free to use at will & at own risk.
// 	Author: 	JCP
// 	Date: 		03.30.06 17:10
// 	Description:    Sets position to x-y location on LCD
// 	Useage:		GotoXY(param 1,param 2)
//
//			param 1: x direction 0-131
//			param 2: y direction 0-131
//
//	Example:	GotoXY(32,17);        
//					
//      Notes:		No bounds checking	
//*******************************************************************************
void GotoXY(unsigned char x, unsigned char y)
{
    SendLcd(LCDCommand, PASET);                             // page start/end ram
    SendLcd(LCDData,y);                                     // Start Page to display to
    SendLcd(LCDData,131);                                   // End Page to display to
                                                            //
    SendLcd(LCDCommand, CASET);                             // column start/end ram
    SendLcd(LCDData,x);                                     // Start Column to display to
    SendLcd(LCDData,131);                                   // End Column to display to
}
//*****************************************************************************
//	Name: 		LCD_PixelPut(unsigned char x, unsigned char y, unsigned char color)
// 	Copyright:	Free to use at will & at own risk.
// 	Author: 	JCP
// 	Date: 		03.30.06 17:10
// 	Description:    Put One Pixel on LCD
// 	Useage:		LCD_PixelPut(param 1,param 2,param 3)
//
//			param 1: x direction 0-131
//			param 2: y direction 0-131
//			param 3: Color of Pixel, RRRGGGBB format
//	Example:	LCD_PixelPut(30,30,0x62)	// put a purple pixel @ 30,30
//					
//      Notes:		No bounds checking	
//*******************************************************************************
void LCD_PixelPut(unsigned char x, unsigned char y, unsigned char color)
{
    GotoXY(x,y);                                            // Set position
    SendLcd(LCDCommand,RAMWR);                              // Now write the pixel to the display
    SendLcd(LCDData,color);                                 // Write the data in the specified Color
}
//*****************************************************************************
//	Name: 		LCD_Fill (unsigned char xs,unsigned char ys,unsigned char width,unsigned char height, unsigned char color)
// 	Copyright:	Free to use at will & at own risk.
// 	Author: 	JCP
// 	Date: 		03.30.06 17:10
// 	Description:    Fills an area starting @ xs,ys to width @ heighth with color  
// 	Useage:		LCD_Fill(param 1,param 2,param 3,param 4,param 5)
//
//			param 1: x start direction: 0-131
//			param 2: y start direction: 0-131
//			param 3: width: 0-131
//			param 4: heighth: 0-131
//			param 5: Color of Pixel, RRRGGGBB format
//
//	Example:	LCD_Fill(0,0,132,132,Black);  // fill entire screen with black color
//					
//      Notes:		No bounds checking	
//*******************************************************************************
void LCD_Fill (unsigned char xs,unsigned char ys,unsigned char width,unsigned char height, unsigned char color)
{
    unsigned char i,j;

    for (i=0;i < height;i++)                        // Number of horizontal lines
    {                                               //
	GotoXY(xs,ys+i);                            // Goto start of fill area (Top Left)
	SendLcd(LCDCommand,RAMWR);                  // Write to display
                                                    //
     	for (j=0;j < width;j++)                     // pixels per line
	{                                           //
            SendLcd(LCDData,color);                 //
	}                                           //
                                                    //
        SendLcd(LCDCommand,NOP);                    // End 
    }
}
///******************************************************************************
//      Name: 		LCD_Funct_A(x1, y1, x2, y2, pixel_on_off, ram_or_lcd)
// 	Copyright:	Free to use at will & at own risk.
// 	Author: 	JCP
// 	Date: 		05.30.06 17:10
//      Description:    Used with LCD line function
//*******************************************************************************
void LCD_Funct_A(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color )
{
    float a,b;
    int x,y;                                              //
                                                                    //
    b = (float)((y2 - y1) + 1) / (float)((x2-x1) + 1);              //
                                                                    //
    a = y1;                                                         //
                                                                    //
    for(x = x1;x <= x2; x++)                                        //
    {                                                               //
                                                                    //
    	for(y=(unsigned char)a;y <= (unsigned char)(a + b); y++)    //
    	{                                                           //
            if(y <= y2)                                             //
            {                                                       //
		GotoXY(x,y);                                        // Goto start of fill area (Top Left) 
		SendLcd(LCDCommand,RAMWR);                                  // write to display 
		SendLcd(LCDData,color);                                     //
            }                                                       //
        }                                                           //
                                                                    //
        a+=b;                                                       //
        
        SendLcd(LCDCommand,NOP);                                    // Data Sheet recomends you send this from time to time
    }                                                               //
}
//******************************************************************************
//  Name:           LCD_Funct_B(x1, y1, x2, y2, pixel_on_off, ram_or_lcd)
//  Copyright:      Free to use at will & at own risk.
//  Author:         JCP
//  Date:           05.30.06 17:10
//  Description:    Used with LCD line function
//*******************************************************************************
void LCD_Funct_B(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color )
{
    float a,b;
    int x,y;                                                        //
                                                                    //
    b = (float)((y2 - y1) + 1) / (float)((x1-x2) + 1);              //
                                                                    //
    a = y1;                                                         //
                                                                    //
    for(x = x1;x >= x2; x--)                                        //
    {                                                               //
    	for(y=(unsigned char)a;y <= (unsigned char)(a + b); y++)    //
    	{                                                           //
            if(y <= y2)                                             //
            {                                                       //
		GotoXY(x,y);                                        // Goto start of fill area (Top Left) 
		SendLcd(LCDCommand,RAMWR);                          // write to display 
		SendLcd(LCDData,color);                             //
            }                                                       //
        }                                                           //
                                                                    //
        a+=b;                                                       //
                                                                    //
        SendLcd(LCDCommand,NOP);                                    // Data Sheet recomends you send this from time to time    
    }                                                               //
}
//******************************************************************************
//  Name:           LCD_Line(x1, y1, x2, y2, pixel_on_off, ram_or_lcd)
//  Copyright:      Free to use at will & at own risk.
//  Author:         JCP
//  Date:           05.30.06 17:10
//  Description:    To draw or remove a line from the LCD
//  Notes:          No bounds checking	
//*******************************************************************************
void LCD_Line(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color)
{

   	if( (x1 <= x2) && (y1 <= y2) )				//
   	{							//
            LCD_Funct_A(x1,y1,x2,y2,color);			//
	}							//
								//
   	else if( (x1 > x2)  && (y1 < y2) )			//
   	{							//
            LCD_Funct_B(x1,y1,x2,y2,color);			//
	}							//
								//
	else if( (x1 >= x2) && (y1 >= y2) )			//
	{							//
            LCD_Funct_A(x2,y2,x1,y1,color);			//
	}							//
								//
	else if( (x1 < x2) && (y1 > y2) )			//
	{							//
            LCD_Funct_B(x2,y2,x1,y1,color);			//
	}							//
								//
}
//******************************************************************************
//  Name:           LCD_Box(x1, y1, x2, y2, pixel_on_off, ram_or_lcd)
//  Copyright:      Free to use at will & at own risk.
//  Author:         JCP
//  Date:           03.30.06 17:10
//  Description:    To draw a BOX on the LCD
//  Notes:          No bounds checking	
//*******************************************************************************
void LCD_Box(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color)
{
    unsigned char x,y;

    x = x1;						//
    y = y1;						//
							//
    while(x <= x2)					//
    {							//
       	LCD_PixelPut(x,y1,color);			//
       	LCD_PixelPut(x,y2,color);			//
							//
	x++;						//
    }							//
							//
    while(y <= y2)					//
    {							//
       	LCD_PixelPut(x1,y,color);			//
       	LCD_PixelPut(x2,y,color);			//
							//
	y++;						//
    }							//
}
//******************************************************************************
// 	Name:      	LCD_Circle
// 	Copyright: 	Free to use at will & at own risk.
// 	Date: 		03.30.06 17:10
// 	Description:    To Draw a Circle on LCD
// 	Useage:		LCD_Circle(param 1,param 2,param 3,param 4)
//
//			param 1: x location on screen, 0 - 127
//			param 2: y location on screen, 0 - 63
//			param 3: diameter of circle
//                      param 4: Color of Pixel, RRRGGGBB format
//
//	Example:        LCD_Circle(40,30,30,LightGreen);
//      Notes:		No bounds checking	
//*******************************************************************************
void LCD_Circle(unsigned char center_x, unsigned char center_y, unsigned int rad, unsigned char color)
{
    unsigned int x;
    unsigned int y;

    x = 0;
    y = rad;

    while (x <= y)
    {
        // We make use of 8 axes of symmetry in a circle.
        // This way we have fewer points to calculate on its circumference.
        LCD_PixelPut (center_x + (unsigned char)x, center_y + (unsigned char)y,color);
        LCD_PixelPut (center_x - (unsigned char)x, center_y + (unsigned char)y,color);
        LCD_PixelPut (center_x + (unsigned char)x, center_y - (unsigned char)y,color);
        LCD_PixelPut (center_x - (unsigned char)x, center_y - (unsigned char)y,color);
        LCD_PixelPut (center_x + (unsigned char)y, center_y + (unsigned char)x,color);
        LCD_PixelPut (center_x - (unsigned char)y, center_y + (unsigned char)x,color);
        LCD_PixelPut (center_x + (unsigned char)y, center_y - (unsigned char)x,color);
        LCD_PixelPut (center_x - (unsigned char)y, center_y - (unsigned char)x,color);

        // This is the most important part of the function.
        // We go to the right in all cases (x++).
        // We need to decide whether to go down (y--).
        // This depends on which point is
        // closest to the path of the circle.
        // Good old Pythagoras will tell us what to do.
        x++;

        if( abs( (x * x) + (y * y) - (rad * rad) ) > abs( (x * x) + ((y-1)*(y-1)) - (rad * rad)) )
        {
            y--;
	}
    }
}
//******************************************************************************
//      Name:      	LCD_Thick_Circle
// 	Copyright: 	Free to use at will & at own risk.
//      Date:           03.30.06 17:10
// 	Description:    To Draw a Definable Thickness Circle on LCD
// 	Useage:		LCD_Thick_Circle(param 1,param 2,param 3,param 4,param 5)
//
//			param 1: x start direction: 0-131
//			param 2: y start direction: 0-131
//                      param 3: diameter: 0-255
//			param 4: Thickness of circle wall in pixels
//			param 5: Color of Pixel, RRRGGGBB format
//
//	Example:       	LCD_Thick_Circle(66,66,30,2,0x62);		
// 			start @ 66,66, with a diamater of 30, 2 pixels thick in purple
//
//      Notes:		No bounds checking	
//*******************************************************************************
void LCD_Thick_Circle(unsigned char center_x, unsigned char center_y,unsigned char diameter, unsigned char thick,unsigned char color)
{
    unsigned int a;

    for(a=0;a < 360;a++)
    {
 	LCD_Circle_Line(center_x,center_y,diameter,(diameter + thick),a,color);
    } 	
}
//******************************************************************************
//  Name:           LCD_Circle_Line
//  Copyright:      Free to use at will & at own risk.
//  Date:           03.30.06 17:10
//  Description:    To Draw a line in a circle, using degrees I.E. like a radar screen
//  Useage:         LCD_Circle_Line(param 1,param 2,param 3,param 4,param 5,param 6)
//                  param 1: x start direction: 0-131
//                  param 2: y start direction: 0-131
//                  param 3: End point of line (usually the size diamater of circle - thickness)
//                  param 4: Start point of line (usually 0, but can go up to diameter)
//                  param 5: angle of line on circles outer rim
//                  param 6: Color of Pixel, RRRGGGBB format
//
//  Example:        LCD_Thick_Circle(66,66,30,2,0x62);          // draw a circle first
//                  LCD_Circle_Line(66,66,28,0,90,LightGreen);
//                  circle center point @ 66,66, with a line length of 28 from center
//                  line start @ 0 or center of circle point 90 deg outward with the color light green
//
//  Notes:          No bounds checking  
//                  degrees_to_radians(x) = ((x) * pi / 180)
//                  radians_to_degrees(x) = ((x) * 180 / pi)
//*******************************************************************************
void LCD_Circle_Line(unsigned char center_x, unsigned char center_y,unsigned char diameter, unsigned char length, float angle,unsigned char lcolor)
{
    unsigned char x,y,x1,y1;						//
                                                                        //
    angle -=90;								// remove lor lanscape mode
									//
    angle *= 0.01745329;						// degrees_to_radians(x) = ((x) * pi / 180)
									//
    x = (char)(length * cos(angle) + center_x);				// figure the center start point of the line
    y = (char)(length * sin(angle) + center_y);				// figure the center start point of the line
									//
    x1 = (char)(diameter * cos(angle) + center_x);			// figure the end point of the line
    y1 = (char)(diameter * sin(angle) + center_y);			// figure the end point of the line
									//
    LCD_Line(x, y, x1, y1, lcolor );					//
}
//******************************************************************************
//  Name:           LCD_String
//  Copyright:      Free to use at will & at own risk.
//  Date:           03.30.06 17:10
//  Description:    To print a string of characters to LCD
//  Useage:         LCD_String(param 1,param 2,param 3,param 4,param 5,param 6)
//
//                  param 1: String data
//                  param 2: pointer to data lookup table (I.E. Font table)
//                  param 3: x start direction: 0-131
//                  param 4: y start direction: 0-131
//                  param 5: Font Color, RRRGGGBB format
//                  param 5: Background color, RRRGGGBB format
//
//  Example:        sprintf(thestring,"Hello World"); 
//                  LCD_String(thestring,FONT6x8,52,25,White,Black);
//          
//                  print "Hello World" in white text on a black background
//
//  Notes:          the first line in the font table must contain 3 variables in this order
//                  Loc[0][0] = font x width
//                  Loc[0][1] = font y height
//                  Loc[0][2] = total data bytes per individual font
//
//                  No bounds checking  
//*******************************************************************************
void LCD_String (char *lcd_string, const char *font_style,unsigned char x, unsigned char y, unsigned char fcolor, unsigned char bcolor)
{
    unsigned int  i,j;                                          //
    unsigned char mask,z,xme,yme,offset;			//
    const char *data;						//
								//
    data = font_style;						// point to the start of the font table
								//
    xme = *data;						// get font x width
    data++;							//
    yme = *data;						// get font y length
    data++;							//
    offset = *data;						// get data bytes per font
								//
    do								//
    {								//
								//
        data =  (font_style + offset) +                         // point to data in table to be loaded
                (offset * (int)(*lcd_string - 32));             //
								//
        for (i=0;i < yme;i++)                                   // 
        {                                                       //
            mask |=0x80;					//
        							//
            for (j=x;j < (x + xme);j++)				//
            {							//
                z= y + i;					//
								//
                if (*data & mask)				// if pixel data then put dot
                {						//
                    LCD_PixelPut (j,z,fcolor);			//
                }						//
                else						// else use background color
                {						//
                    LCD_PixelPut (j,z,bcolor);		   	// 
                }					 	//
								//
                mask>>=1;					//
            }							//
                                                                //
            data++;						//
        }							//
								//
        x+=xme;							//
								//
	lcd_string++;						// next character in string
                                                                //
  	SendLcd(LCDCommand,NOP);					// Its Recomended to send now and then
								//
	}while(*lcd_string !='\0');                            	// keep spitting chars out until end of string
}
//******************************************************************************
// 	Name:      	ShowImage 4096 color mode ONLY!!!
// 	Copyright: 	Free to use at will & at own risk.
// 	Date: 		05.30.06 17:10
// 	Description:    To Display a jpg data color image on lcd
// 	Useage:		ShowImage(param 1,param 2,param 3)
//
//			param 1: x location on screen 
//			param 2: y location on screen
//                      param 3: pointer to starting position of data 
//
//	Example:        ShowImage(0,0,My_Photo);
//      Notes:		No bounds checking	
//*******************************************************************************
void ShowImage_4096(unsigned char x,unsigned char y, const unsigned char  *location)
{
    unsigned char x_max,y_max;
    unsigned char x_logo,y_logo;

    LCD_Set_Resolution(HIGH_RES);                                // Dont forget to switch back to low_res when done!
                                                                //
    x_max =  *location / 2;                                     //
    location++;                                                 //
    y_max = *location;                                          //
    location++;                                                 //
                                                                //
    for(y_logo=0;y_logo <y_max;y_logo++)                        //
    {                                                           //
        GotoXY(x,y + y_logo);                                   //
        SendLcd(LCDCommand,RAMWR);                              // Now write the pixel to the display
                                                                //
        for(x_logo=0;x_logo < x_max;x_logo++)                   //
        {                                                       //
            SendLcd(LCDData,*location);                         // Write the data in the specified Color
            location++;                                         //
            SendLcd(LCDData,*location);                         // Write the data in the specified Color
            location++;                                         //
            SendLcd(LCDData,*location);                         // Write the data in the specified Color
            location++;                                         //
        }                                                       //
    }                                                           //
}                                                               //
//******************************************************************************
// 	Name:      	ShowImage
// 	Copyright: 	Free to use at will & at own risk.
// 	Date: 		05.30.06 17:10
// 	Description:    To Display a jpg data color image on lcd
// 	Useage:		ShowImage(param 1,param 2,param 3)
//
//			param 1: x location on screen
//			param 2: y location on screen
//                      param 3: pointer to starting position of data 
//
//	Example:        ShowImage(0,0,My_Photo);
//      Notes:		No bounds checking	
//*******************************************************************************
void ShowImage(unsigned char x,unsigned char y, const unsigned char  *location)
{
    unsigned char x_max,y_max;
    unsigned char x_logo,y_logo;

    x_max =  *location;                                         //
    location++;                                                 //
    y_max = *location;                                          //
    location++;                                                 //
                                                                //
    for(y_logo=0;y_logo <y_max;y_logo++)                        //
    {                                                           //
        GotoXY(x,y + y_logo);                                   //
        SendLcd(LCDCommand,RAMWR);                              // Now write the pixel to the display
                                                                //
        for(x_logo=0;x_logo < x_max;x_logo++)                   //
        {                                                       //
            SendLcd(LCDData,*location);                         // Write the data in the specified Color
            location++;                                         //
        }                                                       //
    }                                                           //
}                                                               //
















