/*
Board Connections
Olimex ARM Board header (the long strip) --- LCD carrier Board

3.3V     --     Vin
PA11     --     ~CS
PA13     --     SDATA
PA14     --     SCLK
PA15     --     RST
GND      --     GND 

Thats it!

*/

#include <string.h>
#include <ctl_api.h>
#include <stdio.h> 
#include <math.h>
#include "test.h"
//#include "serial.h"
#include "pcf8833u8_lcd.h"
#include "fonts.h"
#include "bitmap.h"
#include "defines.h"


#define CALLSTACKSIZE 16 // this is only required for AVR builds
#define STACKSIZE 256  
#define RAND_MAX 255        

void start_timer(void);
long ticks_per_sec(void);


CTL_TASK_t  main_task, 
            task1_task, 
            task2_task; 
 

CTL_EVENT_SET_t e1;

unsigned char somestring[25];

unsigned task1_stack[1+STACKSIZE+1];
unsigned task2_stack[1+STACKSIZE+1];


//*********************************************************************
//***************************  Task 2  ********************************
//*********************************************************************
void Task_2(void)
{  
    unsigned long z;
    unsigned int x,y;
    unsigned char a,b,c,d,e;

    char seconds,minutes,hours;

    unsigned int nowold,tenths;


    InitLCD();


/*******  Put smiley face up in 4096 color mode  *******/
    LCD_Fill(0,0,132,132,Black);

    LCD_Set_Resolution(HIGH_RES);                        // set 4096 color mode

    ShowImage_4096(0,0,smiley);
    LCD_Set_Resolution(LOW_RES);                        // set 256 color mode
        
    ctl_timeout_wait(ctl_get_current_time()+ 4000);     // wait 4 seconds to view it

/*******  Do some static on screen  *******/

    LCD_Fill(0,0,132,132,Black);
        
    for(z=0;z<100000;z++)
    {
        while( (a = rand()) > 132);
        while( (b = rand()) > 132);
        c = rand();
        LCD_PixelPut(a,b,c);
    }

/*******  Do some lines on screen  *******/
    LCD_Fill(0,0,132,132,Black);
    
    for(z=1;z<300;z++)
    {
        while( (a = rand()) > 132);
        while( (b = rand()) > 132);
        while( (c = rand()) > 132);
        while( (d = rand()) > 132);
        e = rand();                                 // pick color
        
        LCD_Line(a,b,c,d,e);
    	ctl_timeout_wait(ctl_get_current_time()+ 10);    
    }

/*******  Do some Boxes on screen  *******/
    LCD_Fill(0,0,132,132,Black);

    for(z=0;z<300;z++)
    {

        while( (a = rand()) > 132);
        while( (b = rand()) > 132);
        while( (c = rand()) > 132);
        while( (d = rand()) > 132);

        e = rand();                                 // pick color
        LCD_Box(a,b,c,d,e);

        ctl_timeout_wait(ctl_get_current_time()+ 10);    
    }
/*******  Do some Circles on screen  *******/
    LCD_Fill(0,0,132,132,Black);

    for(z=0;z<100;z++)
    {

        while( (a = rand()) > 132);
        while( (b = rand()) > 132);
        while( (c = rand()) > 127);                 // diameter
    
        d = rand();                                 // pick color
        LCD_Circle(a,b,c,d);

        ctl_timeout_wait(ctl_get_current_time()+ 10);    
    }

/*******  Do some Thick Circles on screen  *******/
    LCD_Fill(0,0,132,132,Black);

    for(z=0;z<25;z++)
    {
        while( (a = rand()) > 132);
        while( (b = rand()) > 132);
        while( (c = rand()) > 40);                 // diameter
        while( (d = rand()) > 10);                 // wall thicknes  
        e = rand();                                 // pick color
        LCD_Thick_Circle(a,b,c,d,e);

        ctl_timeout_wait(ctl_get_current_time()+ 1);    
    }

/*******  Do something funkey to wipe screen  *******/
	b=0;

	for(a=0;a<131;a++)
	{
            LCD_Line(a,b,65,65,0x62);
	}
	for(b=0;b<131;b++)
	{
            LCD_Line(a,b,65,65,0x62);
	}
	for(;a>1;a--)
	{
            LCD_Line(a,b,65,65,0x62);
	}
	for(;b>1;b--)
	{
            LCD_Line(a,b,65,65,0x62);
	}

	ctl_timeout_wait(ctl_get_current_time()+ 1000);    

/*******  Show Image scrolling *******/
    LCD_Fill(0,0,132,132,Black);  

    ShowImage(0,50,sparkfun);

    sprintf(somestring,"Thanks SparkFun"); 
    LCD_String(somestring,&FONT8x8F[0][0],5,10,LightGreen,Black);

    ctl_timeout_wait(ctl_get_current_time()+ 2000);     // hold sparkfun image for a bit

    for(y=50;y<140;y++)
    {
        LCD_Line(0,y-1,132,y-1,Black);                  // wipe the white line as it moves down
	ShowImage(0,y,sparkfun);                        // move image to Y location
	ctl_timeout_wait(ctl_get_current_time()+ 25);   // wait a bit
    }

/*******  Run radar in loop with example fonts displayed  *******/
    LCD_Fill(0,0,132,132,Black);

    LCD_Thick_Circle(66,66,30,2,DarkBlue);
    
    y=0;

    while (1)
    { 
	LCD_Circle_Line(66,66,28,0,y,LightGreen);

	ctl_timeout_wait(ctl_get_current_time()+ 1);    

	tenths = ctl_current_time / 1000;

	if(tenths != nowold)
	{
            nowold = tenths;
            
            if(++seconds == 60)
            {
                seconds = 0;
		
                if(++minutes == 60)
                {
                    minutes=0;
                    hours++;
		}
            }
	}


	printf("a=%6lu - b=%6lu - c=%6lu - d=%6lu  :  Time=%lu\r\n",a,b,c,d,ctl_current_time);

	sprintf(somestring,"%05lu",y); 
	LCD_String(somestring,&FONT6x8[0][0],52,25,White,Black);

	sprintf(somestring,"Time:%02u:%02u:%02u",hours,minutes,seconds); 
	LCD_String(somestring,&FONT8x8F[0][0],14,10,DarkRed,Black);

	sprintf(somestring,"Time:%02u:%02u:%02u",hours,minutes,seconds); 
	LCD_String(somestring,&FONT8x16[0][0],14,115,LightGreen,Black);

	LCD_Circle_Line(66,66,28,0,y,Black);

        if(++y==360)
        {
            y=0;
        }  					

	

    }
}
