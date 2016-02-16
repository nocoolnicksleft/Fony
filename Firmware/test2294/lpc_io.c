/*--------------------------------------------------------------------------*/
/* 
 * MP3 player DEMO - MPEG queue management functions
 * Copyright (C) 2006 MXP semicontuctor
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY and WUTHOUT ANY SUPPORT; without even the implied 
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: lpc_io.c,v 1.1 2008/12/19 20:26:55 bseip Exp $
 */
/*--------------------------------------------------------------------------*/
#include  <targets/LPC2148.H>
#include  <string.h>
#include  "lpc_io.h"

#define  bool unsigned char

void tc0 (void) __attribute__ ((interrupt));    // Manage timer Interrupt


/* Setup the Timer Counter 0 Interrupt */
void init_timer ()
{
    VICVectAddr0 = (unsigned long)tc0;            // set interrupt vector in 0
    VICVectCntl0 = 0x20 | 4;                      // use it for Timer 0 Interrupt
    VICIntEnable = 0x00000010;                    // Enable Timer0 Interrupt

    T0MCR = 3;                                    // Interrupt and Reset on MR0
}


/* Timer Counter 0 Interrupt executes sample time @ 60 MHz CPU Clock */
void tc0 (void)
{

    T0IR = 1;                                   // Clear interrupt flag
    VICVectAddr = 0;                            // Acknowledge Interrupt
}

void init_IO(void)
{

#ifdef DEBUG
    //IO1DIR  = 0x00FF0000;      /* P1.16..23 defined as Outputs  */
#endif
    
    //init_timer();
   // __ARMLIB_enableIRQ();     /* Enable global interrupt */
}
