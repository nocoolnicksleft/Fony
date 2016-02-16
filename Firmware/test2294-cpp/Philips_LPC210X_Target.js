/******************************************************************************
  Target Script for Philips LPC2000.

  Copyright (c) 2005 Rowley Associates Limited.

  This file may be distributed under the terms of the License Agreement
  provided with this software.

  THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE
  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 ******************************************************************************/

function Reset()
{
 
  TargetInterface.resetAndStop(50);
  TargetInterface.getICEBreakerRegister(5); /* Clear out Debug Comms Data */

  /*
  TargetInterface.setNSRST(0);
  TargetInterface.setNSRST(1);
  TargetInterface.delay(100);
  TargetInterface.trst();
  TargetInterface.setICEBreakerBreakpoint(0, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x100, 0xF7);
  TargetInterface.waitForDebugState(1000);
  TargetInterface.setICEBreakerBreakpoint(0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x000, 0x00);
  */

  TargetInterface.pokeWord(0xE002C014, 0x0F804924); /* PINSEL2        */  
  TargetInterface.pokeWord(0xFFE00000, 0x1000348F); /* BCFG0 - Flash  */  
  TargetInterface.pokeWord(0xFFE00004, 0x2000040F); /* BCFG1 - RAM    */  

}

/*
 MEMMAP

 00: Boot Loader Mode. Interrupt vectors are re-mapped to Boot Block.
 01: User Flash Mode. Interrupt vectors are not re-mapped and reside in Flash.
 10: User RAM Mode. Interrupt vectors are re-mapped to Static RAM.
 11: User External memory Mode. Interrupt vectors are re-mapped to external memory.

*/

function LPC210X_ResetWithSRAMVectorsAtZero()
{
  Reset();
  TargetInterface.pokeWord(0xE01FC040, 2); // MEMMAP Memory Mapping Control Register = 0000 0010 = User RAM Mode
}

function LPC210X_ResetWithFLASHVectorsAtZero()
{
  Reset();
  TargetInterface.pokeWord(0xE01FC040, 1); // MEMMAP Memory Mapping Control Register = 0000 0001 = User Flash Mode
}

function LPC210X_ResetWithBootLoaderFLASHVectorsAtZero()
{
  Reset();
  TargetInterface.pokeWord(0xE01FC040, 0); // MEMMAP Memory Mapping Control Register = 0000 0000 = Boot Loader Mode
}

function SRAMReset()
{
  LPC210X_ResetWithSRAMVectorsAtZero();
}

function FLASHReset()
{
  LPC210X_ResetWithFLASHVectorsAtZero()
}

